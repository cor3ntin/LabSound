// License: BSD 3 Clause
// Copyright (C) 2012, Google Inc. All rights reserved.
// Copyright (C) 2015+, The LabSound Authors. All rights reserved.

#include "LabSound/core/OscillatorNode.h"
#include "LabSound/core/WaveTable.h"
#include "LabSound/core/AudioArray.h"

#include "internal/Assertions.h"
#include "internal/FFTFrame.h"
#include "internal/VectorMath.h"

#include <algorithm>
#include <iostream>
#include <exception>

const unsigned WaveTableSize = 4096; // This must be a power of two.
const unsigned NumberOfRanges = 36; // There should be 3 * log2(WaveTableSize) 1/3 octave ranges.
const float CentsPerRange = 1200 / 3; // 1/3 Octave.

namespace lab 
{
    
using namespace VectorMath;

WaveTable::WaveTable(float sampleRate, OscillatorType basicWaveform) :
	m_sampleRate(sampleRate), 
	m_waveTableSize(WaveTableSize), 
	m_numberOfRanges(NumberOfRanges),
	m_centsPerRange(CentsPerRange)
{
    float nyquist = 0.5 * m_sampleRate;
    m_lowestFundamentalFrequency = nyquist / maxNumberOfPartials();
    m_rateScale = m_waveTableSize / m_sampleRate;

	generateBasicWaveform(basicWaveform);
}

WaveTable::WaveTable(float sampleRate, OscillatorType basicWaveform, std::vector<float> & real, std::vector<float> & imag)
	: m_sampleRate(sampleRate),
	m_waveTableSize(WaveTableSize), 
	m_numberOfRanges(NumberOfRanges), 
	m_centsPerRange(CentsPerRange)
{
    float nyquist = 0.5 * m_sampleRate;
    m_lowestFundamentalFrequency = nyquist / maxNumberOfPartials();
    m_rateScale = m_waveTableSize / m_sampleRate;

	bool isGood = real.size() == imag.size() && real.size() > 0;

    if (isGood) 
	{
        createBandLimitedTables(&real[0], &imag[0], real.size());
    }
	else
	{
		throw std::runtime_error("Bad FFT data");
	}
}

WaveTable::~WaveTable()
{

}

void WaveTable::waveDataForFundamentalFrequency(float fundamentalFrequency, float* &lowerWaveData, float* &higherWaveData, float& tableInterpolationFactor)
{
    // Negative frequencies are allowed, in which case we alias to the positive frequency.
    fundamentalFrequency = std::abs(fundamentalFrequency);

    // Calculate the pitch range.
    float ratio = fundamentalFrequency > 0 ? fundamentalFrequency / m_lowestFundamentalFrequency : 0.5;
    float centsAboveLowestFrequency = log2f(ratio) * 1200;

    // Add one to round-up to the next range just in time to truncate partials before aliasing occurs.
    float pitchRange = 1 + centsAboveLowestFrequency / m_centsPerRange;

    pitchRange = std::max(pitchRange, 0.0f);
    pitchRange = std::min(pitchRange, static_cast<float>(m_numberOfRanges - 1));

    // The words "lower" and "higher" refer to the table data having the lower and higher numbers of partials.
    // It's a little confusing since the range index gets larger the more partials we cull out.
    // So the lower table data will have a larger range index.
    unsigned rangeIndex1 = static_cast<unsigned>(pitchRange);
    unsigned rangeIndex2 = rangeIndex1 < m_numberOfRanges - 1 ? rangeIndex1 + 1 : rangeIndex1;

    lowerWaveData = m_bandLimitedTables[rangeIndex2]->data();
    higherWaveData = m_bandLimitedTables[rangeIndex1]->data();
    
    // Ranges from 0 -> 1 to interpolate between lower -> higher.
    tableInterpolationFactor = pitchRange - rangeIndex1;
}

unsigned WaveTable::maxNumberOfPartials() const
{
    return m_waveTableSize / 2;
}

unsigned WaveTable::numberOfPartialsForRange(unsigned rangeIndex) const
{
    // Number of cents below nyquist where we cull partials.
    float centsToCull = rangeIndex * m_centsPerRange;

    // A value from 0 -> 1 representing what fraction of the partials to keep.
    float cullingScale = pow(2, -centsToCull / 1200);

    // The very top range will have all the partials culled.
    unsigned numberOfPartials = cullingScale * maxNumberOfPartials();

    return numberOfPartials;
}

// Convert into time-domain wave tables.
// One table is created for each range for non-aliasing playback at different playback rates.
// Thus, higher ranges have more high-frequency partials culled out.
void WaveTable::createBandLimitedTables(const float* realData, const float* imagData, unsigned numberOfComponents)
{
    float normalizationScale = 1.0;

    unsigned fftSize = m_waveTableSize;
    unsigned halfSize = fftSize / 2 + 1;
    unsigned i;
    
    numberOfComponents = std::min(numberOfComponents, halfSize);

    m_bandLimitedTables.reserve(m_numberOfRanges);

    for (unsigned rangeIndex = 0; rangeIndex < m_numberOfRanges; ++rangeIndex) {
        // This FFTFrame is used to cull partials (represented by frequency bins).
        FFTFrame frame(fftSize);
        float* realP = frame.realData();
        float* imagP = frame.imagData();

        // Copy from loaded frequency data and scale.
        float scale = fftSize;
        vsmul(realData, 1, &scale, realP, 1, numberOfComponents);
        vsmul(imagData, 1, &scale, imagP, 1, numberOfComponents);

        // If fewer components were provided than 1/2 FFT size, then clear the remaining bins.
        for (i = numberOfComponents; i < halfSize; ++i) {
            realP[i] = 0;
            imagP[i] = 0;
        }
        
        // Generate complex conjugate because of the way the inverse FFT is defined.
        float minusOne = -1;
        vsmul(imagP, 1, &minusOne, imagP, 1, halfSize);

        // Find the starting bin where we should start culling.
        // We need to clear out the highest frequencies to band-limit the waveform.
        unsigned numberOfPartials = numberOfPartialsForRange(rangeIndex);

        // Cull the aliasing partials for this pitch range.
        for (i = numberOfPartials + 1; i < halfSize; ++i) {
            realP[i] = 0;
            imagP[i] = 0;
        }

        // Clear packed-nyquist if necessary.
        if (numberOfPartials < halfSize) {
            imagP[0] = 0;
		}

		// Clear nyquist if necessary {
        if (numberOfPartials < halfSize) {
            realP[halfSize-1] = 0;
		}
		
		// Clear values which have no effect
		imagP[0] = 0;
        imagP[halfSize-1] = 0;

        // Clear any DC-offset.
        realP[0] = 0;

        // Create the band-limited table.
        m_bandLimitedTables.push_back(std::unique_ptr<lab::AudioFloatArray>(new lab::AudioFloatArray(m_waveTableSize)));

        // Apply an inverse FFT to generate the time-domain table data.
        float* data = m_bandLimitedTables[rangeIndex]->data();
        frame.doInverseFFT(data);

        // For the first range (which has the highest power), calculate its peak value then compute normalization scale.
        if (!rangeIndex) {
            float maxValue;
            vmaxmgv(data, 1, &maxValue, m_waveTableSize);

            if (maxValue)
                normalizationScale = 1.0f / maxValue;
        }

        // Apply normalization scale.
        vsmul(data, 1, &normalizationScale, data, 1, m_waveTableSize);          
    }
}

void WaveTable::generateBasicWaveform(OscillatorType shape)
{
    unsigned fftSize = waveTableSize();
    unsigned halfSize = fftSize / 2;

    lab::AudioFloatArray real(halfSize);
    lab::AudioFloatArray imag(halfSize);
    float* realP = real.data();
    float* imagP = imag.data();

    // Clear DC and Nyquist.
    realP[0] = 0;
	imagP[0] = 0;
    
    const float piFloat = float(3.14159265358979323846);

    for (unsigned n = 1; n < halfSize; ++n) 
	{
        float piFactor = 2 / (n * piFloat);

        // All waveforms are odd functions with a positive slope at time 0. Hence the coefficients
        // for cos() are always 0.

        // Fourier coefficients according to standard definition:
        // b = 1/pi*integrate(f(x)*sin(n*x), x, -pi, pi)
        //   = 2/pi*integrate(f(x)*sin(n*x), x, 0, pi)
        // since f(x) is an odd function.

        float b; // Coefficient for sin().

        // Calculate Fourier coefficients depending on the shape. Note that the overall scaling
        // (magnitude) of the waveforms is normalized in createBandLimitedTables().
        switch (shape) {
        case OscillatorType::SINE:
            // Standard sine wave function.
            b = (n == 1) ? 1 : 0;
            break;
        case OscillatorType::SQUARE:
            // Square-shaped waveform with the first half its maximum value and the second half its
            // minimum value.
            //
            // See http://mathworld.wolfram.com/FourierSeriesSquareWave.html
            //
            // b[n] = 2/n/pi*(1-(-1)^n)
            //      = 4/n/pi for n odd and 0 otherwise.
            //      = 2*(2/(n*pi)) for n odd
            b = (n & 1) ? 2 * piFactor : 0;
            break;
        case OscillatorType::SAWTOOTH:
            // Sawtooth-shaped waveform with the first half ramping from zero to maximum and the
            // second half from minimum to zero.
            //
            // b[n] = -2*(-1)^n/pi/n
            //      = (2/(n*pi))*(-1)^(n+1)
            b = piFactor * ((n & 1) ? 1 : -1);
            break;
        case OscillatorType::TRIANGLE:
            // Triangle-shaped waveform going from 0 at time 0 to 1 at time pi/2 and back to 0 at
            // time pi.
            //
            // See http://mathworld.wolfram.com/FourierSeriesTriangleWave.html
            //
            // b[n] = 8*sin(pi*k/2)/(pi*k)^2
            //      = 8/pi^2/n^2*(-1)^((n-1)/2) for n odd and 0 otherwise
            //      = 2*(2/(n*pi))^2 * (-1)^((n-1)/2)
            if (n & 1) {
                b = 2 * (piFactor * piFactor) * ((((n - 1) >> 1) & 1) ? -1 : 1);
            } else {
                b = 0;
            }
            break;
        default:
            ASSERT_NOT_REACHED();
            b = 0;
            break;
        }

        realP[n] = 0;
        imagP[n] = b;
    }

    createBandLimitedTables(realP, imagP, halfSize);
}

} // namespace lab

