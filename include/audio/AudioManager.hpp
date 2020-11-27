#ifndef AUDIO_MANAGER_H_
#define AUDIO_MANAGER_H_

#include <vector>

namespace monkeysworld {
namespace audio {

/**
 *  Manages playback of audio files.
 */ 
class AudioManager {
 public:
  /**
   *  Constructs a new AudioManager. Sets up the portaudio callback.
   */ 
  AudioManager();

 private:

  /**
   *  Reads n bytes from all currently active buffers.
   */ 
  int ReadFromBuffers(int n, float* output);
  // audio buffer vector
  
  
};

}
}

#endif  // AUDIO_MANAGER_H_