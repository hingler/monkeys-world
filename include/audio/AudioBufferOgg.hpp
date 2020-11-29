#ifndef AUDIO_BUFFER_OGG_H_
#define AUDIO_BUFFER_OGG_H_

#include <audio/AudioBuffer.hpp>
#include <_stb_libs/stb_vorbis.h>

namespace monkeysworld {
namespace audio {

/**
 *  Implements AudioBuffer for ogg files.
 *  Ctor from file name only for now
 */ 
class AudioBufferOgg : public AudioBuffer {

 public:
  AudioBufferOgg(int capacity, const std::string& filename);
  /**
   *  Specialization for ogg format
   */ 
  int WriteFromFile(int n) override;

  ~AudioBufferOgg();
  AudioBufferOgg& operator=(const AudioBufferOgg& other) = delete;
  AudioBufferOgg& operator=(AudioBufferOgg&& other);
  AudioBufferOgg(const AudioBufferOgg& other) = delete;
  AudioBufferOgg(AudioBufferOgg&& other);
 private:
  stb_vorbis* vorbis_file_;             // the vorbis file assc'd w this buffer
};

}
}

#endif  // AUDIO_BUFFER_OGG_H_