#ifndef AUDIO_BUFFER_H_
#define AUDIO_BUFFER_H_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

#define CACHE_LINE 64

namespace monkeysworld {
namespace audio {

/**
 *  Inheritable class for audio buffers.
 *  Used by AudioManager to source audio samples from file.
 *  Typically: manager will construct on demand, then choose to populate from cache.
 *  Then, it will start the write thread, which will take care of writing from there.
 *  Only one thread (the file writer thread) will write at a time,
 *  and only one thread will read at a time.
 */ 
class AudioBuffer {
 public:
  /**
   *  Creates new AudioBuffer with capacity `size` bytes
   */ 
  AudioBuffer(int capacity);

  // note: determine if "peek" is necessary later :)
  // PA suggests avoiding mutex locks as they lead to concurrency issues.
  // at the moment peek is not necessary, but we could need it later! (probably not lol)
  // use the callback to write some data to another buffer 

  /**
   *  Reads `n` samples from the buffer and moves them to `output`.
   *  Advances the read head.
   *  @param n - number of samples to read.
   *  @param output - buffer which stores output.
   *  @returns number of samples which could be outputted.
   */ 
  int Read(int n, float* output);

  /**
   *  Reads `n` samples from the buffer and moves them to `output`.
   *  Does not advance the read head.
   *  @param n - number of samples to read.
   *  @param output - buffer which stores output.
   *  @returns number of samples which could be outputted.
   */ 
  int Peek(int n, float* output);

  /**
   *  Writes `n` samples from `input` to the buffer.
   *  Advances the write head.
   */ 
  int Write(int n, float* input);

  /**
   *  Starts up the thread which writes to the buffer from a file.
   */
  bool StartWriteThread(); 

  /**
   *  Implementation-defined function which reads
   *  `n` bytes from the buffer file at the sample currently represented  
   *  by the buffer, to the buffer.
   */ 
  virtual int WriteFromFile(int n) = 0;

  /**
   *  Audio buffer dtor.
   */ 
  ~AudioBuffer();

  
 protected:
  // implementation shouldn't require locks!
  const int capacity_;
  float* buffer_;                        // set up by AudioBuffer ctor.
  char CACHE_BREAK_R_[CACHE_LINE];      // separates read from buffer
  std::atomic<uint64_t> bytes_read_;    // read header
  uint64_t last_write_polled_;          // last write value polled
  char CACHE_BREAK_W_[CACHE_LINE];      // separates write from read
  std::atomic<uint64_t> bytes_written_; // write header
  uint64_t last_read_polled_;           // last read value polled

  std::condition_variable write_cv_;    // cv used to signal write thread
  std::thread write_thread_;            // write thread 
  std::atomic_bool running_;            // true if thread is running
  std::atomic_flag write_thread_flag_;  // flag which signals early termination of write thread
  std::mutex write_lock_;               // lock used by wait func on write thread
 private:
  /**
   *  Function which writes to the buffer.
   */ 
  void WriteThreadFunc();

};

}
}

#endif  // AUDIO_BUFFER_H_