#ifndef AUDIO_MANAGER_H_
#define AUDIO_MANAGER_H_

#include <memory>
#include <vector>

#include <AudioBuffer.hpp>
#include <portaudio.h>

#define AUDIO_MGR_MAX_BUFFER_COUNT 256

namespace monkeysworld {
namespace audio {

/**
 *  Enum for filetypes which can be loaded
 */ 
enum AudioFiletype {
  OGG
};

/**
 *  Manages playback of audio files.
 */ 
class AudioManager {
 public:
  /**
   *  Constructs a new AudioManager. Sets up the portaudio callback.
   */ 
  AudioManager();
  
  /**
   *  Adds a file to the audio buffer.
   *  @param filename - file to open
   *  @param file_type - the reader to use to open this file.
   *  @returns an integer which can be used to update the state of the file.
   */ 
  int AddFileToBuffer(const std::string& filename, AudioFiletype file_type);

  /**
   *  Removes a stream which has already been created.
   *  Should only be called by portaudio.
   *  @param stream - Reference to a stream which has already been instantiated. 
   */ 
  void RemoveFileFromBuffer(int stream);

  /**
   *  Reads n samples from all currently active buffers.
   *  @param n - number of samples to read.
   *  @param output - buffer to output result to.
   */ 
  void ReadFromBuffers(int n, float* output);

 private:
  // TODO -- expansion:
  //    - tag certain buffers in some way, so that we can fuck with music/sfx volume
  //    - those categories would be set when the samples themselves are queued
  //    - 
  struct buffer_info {
    AudioBuffer* buffer;        // ptr to buffer, or null if not allocated yet
    std::atomic_int8_t status;  // communicates how the callback is treating the buffer
                                // if 0: buffer is out of use and/or unallocated
                                // if 1: buffer is in use
                                // if 2: buffer will be ignored on future callbacks -- callback should set to 0

                                // we can also add here:
                                //  - looping
                                //  - filters and shit!
  };


  /**
   *  Function called by portauio.
   */ 
  static void CallbackFunc(const void* input,
                           void* output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void* userData);

  
  // audio buffer vector
  buffer_info buffers_[AUDIO_MGR_MAX_BUFFER_COUNT];

};

}
}

#endif  // AUDIO_MANAGER_H_