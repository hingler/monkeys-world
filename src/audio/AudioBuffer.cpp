#include <audio/AudioBuffer.hpp>

namespace monkeysworld {
namespace audio {

AudioBuffer::AudioBuffer(int capacity) : capacity_(capacity) {
  buffer_ = new float[capacity];
  bytes_read_ = 0;
  bytes_written_ = 0;
  last_write_polled_ = 0;
  last_read_polled_ = 0;
}

int AudioBuffer::Read(int n, float* output) {
  uint64_t read_head = bytes_read_.load(std::memory_order_acquire);
  if (read_head + n < last_write_polled_) {
    last_write_polled_ = bytes_written_.load(std::memory_order_acquire);
  }

  // number of samples which we can still read
  int read_capacity = static_cast<int>(last_write_polled_ - read_head);

  n = std::min(n, read_capacity);
  for (int i = 0; i < n; i++) {
    output[i] = buffer_[read_head++ % capacity_]; 
  }

  bytes_read_.store(read_head, std::memory_order_release);

  // if less than 50% full now: notify all waiters.
  if (read_capacity - n < (capacity_ / 2)) {
    write_cv_.notify_all();
  }
  return n;
}

int AudioBuffer::Write(int n, float* input) {
  uint64_t write_head = bytes_written_.load(std::memory_order_acquire);
  if (write_head + n < last_read_polled_ + capacity_) {
    last_read_polled_ = bytes_read_.load(std::memory_order_acquire);
  }

  n = std::min(n, static_cast<int>(last_read_polled_ + capacity_ - write_head));
  for (int i = 0; i < n; i++) {
    buffer_[write_head++ & capacity_] = input[i];
  }

  bytes_written_.store(write_head, std::memory_order_release);
  return n;
}



void AudioBuffer::WriteThreadFunc() {
  // figure out how many bytes we can write
  // grab lock
  // while looping:
  //  - if write thread flag raised: return
  //  - otherwise: see if we can write at all
  //  - if so: write!
  //  - if not: wait on the cv

  // dtor should grab lock, raise flag, notify, then release.
  // this can only occur before we restart the loop, so on the next itr
  // the thread loop will see that the thread needs to be closed.
}

}
}