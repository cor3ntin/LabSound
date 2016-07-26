#include "AudioDestinationQt.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <QIODevice>
#include <QVector>
#include <qdebug.h>
#include "LabSound/core/AudioIOCallback.h"
#include "LabSound/core/AudioNode.h"
#include "internal/FloatConversion.h"
#include "internal/VectorMath.h"
#include <cmath>

namespace lab {

AudioDestination * AudioDestination::MakePlatformAudioDestination(AudioIOCallback & callback, unsigned numberOfOutputChannels, float sampleRate)
{
    return new AudioDestinationQt(callback, numberOfOutputChannels, sampleRate);
}

float AudioDestination::hardwareSampleRate() {
    return 44100;
}

unsigned long AudioDestination::maxChannelCount() {
    return 2;
}

}

const float kLowThreshold = -1.0f;
const float kHighThreshold = 1.0f;

class AudioBuffer : public QIODevice {
public:
    AudioBuffer(lab::AudioIOCallback& callback, QObject* parent = nullptr)
        : QIODevice(parent)
        , m_callback(callback) {

    }

    void setSampleRate(float sampleRate) {
        m_renderBus.setSampleRate(sampleRate);
    }

    bool isSequential() const override  { return true; }
    bool open(OpenMode mode) override {
        if(mode != QIODevice::ReadOnly)
            return false;
        QIODevice::open(mode);
        return true;
    }

    virtual qint64 readData(char *data, qint64 maxlen) override {
       float* fdata = (float*)data;
       size_t fmax  = maxlen / sizeof (float);

       size_t pos = 0;
       size_t frameToProcesss = std::floor( fmax / m_renderBus.numberOfChannels());
       while(frameToProcesss > 0) {
           m_callback.render(0, &m_renderBus, lab::AudioNode::ProcessingSizeInFrames);
           size_t framesAvailable = std::min(
                       m_renderBus.length(),
                       std::min(frameToProcesss, size_t(lab::AudioNode::ProcessingSizeInFrames))
           );
           if(framesAvailable == 0)
               return -1;

           for(size_t frame = 0; frame < framesAvailable; ++frame) {
               for (unsigned channelIndex = 0; channelIndex < 2; ++channelIndex) {
                   const float* source = m_renderBus.channel(channelIndex)->data() + frame;
                   fdata[pos++] = *source;
               }
           }
           frameToProcesss -= framesAvailable;
       }
       return pos * sizeof (float);
    }

    virtual qint64 writeData(const char *, qint64 ) override {
        return -1;
    }


private:
    lab::AudioBus m_renderBus = {2, lab::AudioNode::ProcessingSizeInFrames, true};
    lab::AudioIOCallback& m_callback;
};

AudioDestinationQt::AudioDestinationQt(lab::AudioIOCallback& callback, unsigned numberOfOutputChannels, float sampleRate)
   : m_buffer(new AudioBuffer(callback))
   , m_callback(callback) {

    QAudioFormat f;
    f.setSampleRate(sampleRate);
    f.setChannelCount(numberOfOutputChannels);
    f.setByteOrder(QAudioFormat::LittleEndian);
    f.setSampleType(QAudioFormat::Float);
    f.setSampleSize(sizeof(float) * 8);
    f.setCodec("audio/pcm");

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(f)) {
        qWarning() << "Default format not supported - trying to use nearest";
        f = info.nearestFormat(f);
    }

    m_audioOutput.reset(new QAudioOutput(f));
    m_audioOutput->setBufferSize(14105);

    m_buffer->setSampleRate(sampleRate);
    m_buffer->open(QIODevice::ReadOnly);
}

AudioDestinationQt::~AudioDestinationQt() {

}

void AudioDestinationQt::start() {
    m_audioOutput->start(m_buffer.get());
}

void AudioDestinationQt::stop() {
    m_audioOutput->stop();
}

bool AudioDestinationQt::isPlaying() {
    return m_audioOutput->state() == QAudio::ActiveState;
}

// Sample-rate conversion may happen in AudioDestination to the hardware sample-rate
float AudioDestinationQt::sampleRate() const {
    return m_audioOutput->format().sampleRate();
}
