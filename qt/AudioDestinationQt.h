#ifndef _H_LABSOUND_AUDIODESTINATION_QT_
#define _H_LABSOUND_AUDIODESTINATION_QT_

#include "LabSound/core/AudioNode.h"
#include "LabSound/../../src/internal/AudioBus.h"
#include "LabSound/../../src/internal/AudioDestination.h"
#include <memory>

class QIODevice;
class QAudioOutput;
class AudioBuffer;

class AudioDestinationQt : public lab::AudioDestination {

public:
    AudioDestinationQt(lab::AudioIOCallback&, unsigned numberOfOutputChannels, float sampleRate);
    ~AudioDestinationQt();

    void start() override;
    void stop()  override;
    bool isPlaying() override;

    // Sample-rate conversion may happen in AudioDestination to the hardware sample-rate
    float sampleRate() const override;

private:
    std::unique_ptr<AudioBuffer> m_buffer;
    std::unique_ptr<QAudioOutput> m_audioOutput;
    lab::AudioIOCallback & m_callback;
};

#endif
