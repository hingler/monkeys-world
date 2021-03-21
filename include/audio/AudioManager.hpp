#ifndef AUDIO_MANAGER_H_
#define AUDIO_MANAGER_H_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include <audio/AudioBuffer.hpp>
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

struct AudioStream {
  const int handle;
  operator int() const { return handle; };
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
  AudioStream AddFileToBuffer(const std::string& filename, AudioFiletype file_type);

  /**
   *  Begins playback of a new buffer.
   *  @param buffer - the buffer to be played.
   */ 
  AudioStream AddBuffer(std::shared_ptr<AudioBuffer> buffer);

  /**
   *  Removes a stream which has already been created.
   *  Should only be called by portaudio.
   *  @param stream - Reference to a stream which has already been instantiated. 
   */ 
  int RemoveFileFromBuffer(AudioStream stream);

  ~AudioManager();
  AudioManager& operator=(const AudioManager& other) = delete;
  AudioManager& operator=(AudioManager&& other) = delete;
  AudioManager(const AudioManager& other) = delete;
  AudioManager(AudioManager&& other) = delete;

 private:
  // TODO -- expansion:
  //    - tag certain buffers in some way, so that we can fuck with music/sfx volume
  //    - those categories would be set when the samples themselves are queued
  //    - add looping!
  enum buffer_status {
    AVAILABLE,
    ALLOCATING,
    USED,
    DELETING
  };

  struct buffer_info {
    std::shared_ptr<AudioBuffer> buffer;  // ptr to buffer, or null if not allocated yet
    std::atomic<buffer_status> status;    // communicates how the callback is treating the buffer
                                          // if 0: buffer is out of use and/or unallocated
                                          // if 1: buffer is in use
                                          // if 2: buffer will be ignored on future callbacks -- callback should set to 0

                                // we can also add here:
                                //  - looping
                                //  - filters and shit!
  };

  struct queue_info {
    std::string filename; // path to desired file
    AudioFiletype type;   // type of file being added
    int index;            // index of new buffer
  };

  /**
   *  @param index - populated with new index. Set to -1 if spot could not be found.
   *  @returns a pointer to a valid buffer info spot.
   */ 
  buffer_info* GetOpenBufferSpot(int& index);

  /**
   *  Function called by portaudio.
   */ 
  static int CallbackFunc(const void* input,
                           void* output,
                           unsigned long frameCount,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void* userData);

  /**
   *  Reads from the buffer creation queue and sets up files for playback
   */ 
  void QueueThreadfunc();

  // audio buffer vector
  buffer_info buffers_[AUDIO_MGR_MAX_BUFFER_COUNT];
  std::mutex buffer_info_write_lock_;

  std::queue<queue_info> buffer_creation_queue_;    // queue of buffers to set up
  std::mutex buffer_queue_lock_;                    // lock for queue
  std::thread buffer_creation_thread_;              // thread to handle buffer creation
  std::atomic_flag buffer_thread_flag_;
  std::condition_variable buffer_thread_cv_;         // cv for creation thread

  PaStream* stream_;

};

}
}

#endif  // AUDIO_MANAGER_H_