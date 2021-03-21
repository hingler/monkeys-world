#ifndef AUDIO_BUFFER_H_
#define AUDIO_BUFFER_H_

#include <audio/AudioBufferPacket.hpp>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

// pulled from an implementation i saw a long ass time ago.
// source is probably cited on that music visualizer i made a couple years ago.
// prevents cache collisions.
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
   *  @param output_left - left speaker output
   *  @param output_right - right speaker output
   *  @returns number of samples which could be outputted.
   */ 
  int Read(int n, float* output_left, float* output_right);

  /**
   *  Reads `n` samples from the buffer and adds them to the values contained in `output`.
   *  @param n - number of samples to read.
   *  @param output_left - left speaker output
   *  @param output_right - right speaker output
   *  @returns number of samples which could be outputted.
   */
  int ReadAdd(int n, float* output_left, float* output_right);
  /**
   *  Same as readadd, but samples are interleaved in output.
   *  output must be capable of storing 2 * n samples.
   */ 
  int ReadAddInterleaved(int n, float* output);

  /**
   *  Reads `n` samples from the buffer and moves them to `output`.
   *  Does not advance the read head.
   *  @param n - number of samples to read.
   *  @param output_left - left speaker output
   *  @param output_right - right speaker output
   *  @returns number of samples which could be outputted.
   */ 
  int Peek(int n, float* output_left, float* output_right);


  /**
   *  Writes `n` samples from `input` to the buffer, from left and right channels.
   *  Advances the write head.
   */ 
  int Write(int n, float* input_left, float* input_right);

  /**
   *  Tells the buffer to loops its source.
   */ 
  void ToggleLoop();

  /**
   *  @returns true if the audio is looped
   */ 
  bool IsLooped();

  /**
   *  Sets the gain of this audio buffer, in dB.
   *  @param db - the updated gain.
   */ 
  void SetGain(float db);

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
   *  Implementation defined function which
   *  returns `true` if the file underlying this buffer has been exhausted.
   */ 
  virtual bool EndOfFile() = 0;

  uint64_t GetBytesWritten() {
    return bytes_written_.load(std::memory_order_acquire);
  }

  /**
   *  Terminates the write thread.
   */ 
  void DestroyWriteThread();

  /**
   *  Audio buffer dtor.
   *  NOTE: Since the write thread makes use of resources allocated by subclasses,
   *  it is necessary for implementations to clean up the write thread themselves.
   * 
   *  Cool trick: https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
   *  Virtual dtors are intended for polymorphic use cases -- ie when we have pointers to subclasses
   *  that we intend to dealloc ourselves.
   */ 
  virtual ~AudioBuffer();
  AudioBuffer& operator=(const AudioBuffer& other) = delete;
  AudioBuffer& operator=(AudioBuffer&& other);
  AudioBuffer(const AudioBuffer& other) = delete;
  AudioBuffer(AudioBuffer&& other);
  

  
 protected:

  /**
   *  @param request - the number of frames requested.
   *  @returns an audiobufferpacket containing (at most) the number of bytes requested.
   */ 
  AudioBufferPacket GetBufferSpace(uint64_t request);


  /**
   *  Seeks the underlying file so that it matches the write head.
   *  Useful for caching.
   */ 
  virtual void SeekFileToWriteHead() = 0;


 private:
  int capacity_;
  float* buffer_l_;                     // left buffer
  float* buffer_r_;                     // right buffer

  char CACHE_BREAK_R_[CACHE_LINE];        // separates read from buffer
  std::atomic<uint64_t> bytes_read_;      // read header
  uint64_t last_write_polled_;            // last write value polled
  std::atomic<float> gain_;               // gain of this buffer, in dB.

  char CACHE_BREAK_W_[CACHE_LINE];        // separates write from read
  std::atomic<uint64_t> bytes_written_;   // write header
  uint64_t last_read_polled_;             // last read value polled

  std::condition_variable write_cv_;    // cv used to signal write thread
  std::thread write_thread_;            // write thread 
  std::atomic_bool running_;            // true if thread is running
  std::atomic_flag write_thread_flag_;  // flag which signals early termination of write thread
  std::mutex write_lock_;               // lock used by wait func on write thread
  
  bool looped_;

  /**
   *  Function which writes to the buffer.
   */ 
  void WriteThreadFunc();
  float GetGainAsAmplitude();

};

}
}

#endif  // AUDIO_BUFFER_H_