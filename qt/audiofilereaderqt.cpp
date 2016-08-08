#include "internal/AudioFileReader.h"
#include <QAudioBuffer>
#include <QFile>
#include <qdebug.h>
#include "oggutils.cpp"
#include <opusfile.h>
#include "internal/AudioBus.h"

namespace lab {

/*std::unique_ptr<AudioBus> MakeBusFromOggVorbis(QIODevice* device, bool mixToMono, float sampleRate) {
   OggVorbis_File handle;
   ov_callbacks callbacks = {
       detail::AR_readOgg,  detail::AR_seekOgg,  detail::AR_closeOgg,  detail::AR_tellOgg
   };
   if (ov_test_callbacks((void*)device, &handle, nullptr, 0, callbacks) != 0) {
       qWarning() << "Not a valid ogg vorbis file";
       return {};
   }
   device->seek(0);

   int r = ov_open_callbacks((void*)device, &handle, nullptr, 0, callbacks);
   if ( r < 0) {
       qWarning() << "Can not open this ogg file" << r;
       return {};
   }

   vorbis_info *ovInfo = ov_info(&handle, -1);

   auto samples = ov_pcm_total(&handle, -1);
   int channels  = ovInfo->channels;
   auto bus = std::unique_ptr<AudioBus>(new AudioBus(channels, samples));
   bus->setSampleRate(ovInfo->rate);

   float **buffer;
   size_t totalFramesRead = 0;
   size_t framesRemaining = samples;


   while(framesRemaining > 0) {
       int bitstream = 0;
       int64_t framesRead = ov_read_float(&handle, &buffer, framesRemaining, nullptr);

       // EOF
       if(!framesRead)
           break;

       if (framesRead < 0) {
           // Probably OV_HOLE, OV_EBADLINK, OV_EINVAL. Log warning here.
           continue;
       }

       //Probably a very bad memory access pattern, should improve perfs there
       for (int i = 0; i < framesRead; ++i) {
           for(int ch = 0; ch < ovInfo->channels; ch++) {
               *(bus->channel(ch)->mutableData() + totalFramesRead) = (float)buffer[ch][i];
           }
           totalFramesRead++;
       }
       framesRemaining -= framesRead;
   }

   ov_clear(&handle);
   return AudioBus::createBySampleRateConverting(bus.get(), mixToMono, sampleRate);
}*/


std::unique_ptr<AudioBus> MakeBusFromOggOpus(QIODevice* device, bool mixToMono, float sampleRate) {

   OpusFileCallbacks callbacks = {
       &detail::readOggOpus,  detail::seekOggOpus, detail::tellOggOpus,  detail::closeOggOpus
   };

   int err;
   std::unique_ptr<OggOpusFile, void(*)(OggOpusFile*)> handle(op_open_callbacks((void*)device, &callbacks, 0, 0, &err), &op_free);
   if ( err != 0 || ! handle ) {
       qWarning() << "Can not open this ogg file" << err;
       return {};
   }


   auto samples  = op_pcm_total(handle.get(), -1);
   int channels  = 2; //op_channel_count(handle.get(), -1);
   auto bus = std::unique_ptr<AudioBus>(new AudioBus(channels, samples));

   auto rate = op_bitrate(handle.get(), -1);

   bus->setSampleRate(48000);


   size_t totalFramesReadL = 0, totalFramesReadR = 0;
   size_t framesRemaining = samples;
   float buffer[11520];


   while(true) {
       int64_t framesRead = op_read_float_stereo(handle.get(), buffer, 11520);

       if (framesRead <= 0) {
           break;
       }

       auto const chanL = bus->channel(0)->mutableData();
       for (int i = 0; i < (framesRead * 2) -1; i+=2) {
           chanL[totalFramesReadL++] = buffer[i];
       }
       auto const chanR = bus->channel(1)->mutableData();
       for (int i = 1; i < (framesRead * 2); i+=2) {
           chanR[totalFramesReadR++] = buffer[i];
       }
       framesRemaining -= framesRead * 2;
   }

   return AudioBus::createBySampleRateConverting(bus.get(), mixToMono, sampleRate);
}

std::unique_ptr<AudioBus> MakeBusFromFile(const QString & file, bool mixToMono, float sampleRate) {
    QFile f(file);
    if(!f.open(QIODevice::ReadOnly))
        return {};
    if(file.endsWith(".opus"))
        return MakeBusFromOggOpus(&f, mixToMono, sampleRate);
    return {};
}

std::unique_ptr<AudioBus> MakeBusFromFile(const char * filePath, bool mixToMono, float sampleRate) {
    return MakeBusFromFile(QString::fromUtf8(filePath), mixToMono, sampleRate);
}

std::unique_ptr<AudioBus> MakeBusFromMemory(const std::vector<uint8_t> &, std::string, bool, float) {

    return {}; // 'NOT IMPLEMENTED'
}

}
