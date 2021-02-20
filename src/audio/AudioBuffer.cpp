#include <audio/AudioBuffer.hpp>
#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace audio {

AudioBuffer::AudioBuffer(int capacity) : capacity_(capacity) {
  buffer_l_ = new float[capacity];
  buffer_r_ = new float[capacity];
  bytes_read_ = 0;
  bytes_written_ = 0;
  bytes_allocated_ = 0;
  last_write_polled_ = 0;
  last_read_polled_ = 0;
  running_ = false;
  write_thread_flag_.test_and_set();
}



int AudioBuffer::Read(int n, float* output_left, float* output_right) {
  int increment = Peek(n, output_left, output_right);
  bytes_read_.fetch_add(increment, std::memory_order_release);
  return increment;
}

int AudioBuffer::ReadAddInterleaved(int n, float* output) {
  uint64_t read_head = bytes_read_.load(std::memory_order_acquire);
  if (read_head + n >= last_write_polled_) {
    last_write_polled_ = bytes_written_.load(std::memory_order_acquire);
  }

  int read_size = static_cast<int>(last_write_polled_ - read_head);

  n = std::min(n, read_size);
  for (int i = 0; i < n; i++) {
    *(output++) += buffer_l_[read_head % capacity_];
    *(output++) += buffer_r_[read_head++ % capacity_];
  }

  if (read_size < (capacity_ / 2)) {
    write_cv_.notify_all();
  }

  bytes_read_.fetch_add(n, std::memory_order_release);

  return n;
}

int AudioBuffer::ReadAdd(int n, float* output_left, float* output_right) {
  uint64_t read_head = bytes_read_.load(std::memory_order_acquire);
  if (read_head + n >= last_write_polled_) {
    last_write_polled_ = bytes_written_.load(std::memory_order_acquire);
  }

  int read_size = static_cast<int>(last_write_polled_ - read_head);

  n = std::min(n, read_size);
  for (int i = 0; i < n; i++) {
    output_right[i] += buffer_r_[read_head % capacity_];
    output_left[i] += buffer_l_[read_head++ % capacity_];
  }

  if (read_size < (capacity_ / 2)) {
    write_cv_.notify_all();
  }

  bytes_read_.fetch_add(read_size, std::memory_order_release);
  return n;
}

int AudioBuffer::Peek(int n, float* output_left, float* output_right) {
  uint64_t read_head = bytes_read_.load(std::memory_order_acquire);
  if (read_head + n >= last_write_polled_) {
    last_write_polled_ = bytes_written_.load(std::memory_order_acquire);
  }

  // number of samples which we can still read
  int read_size = static_cast<int>(last_write_polled_ - read_head);

  n = std::min(n, read_size);
  for (int i = 0; i < n; i++) {
    output_right[i] = buffer_r_[read_head % capacity_];
    output_left[i] = buffer_l_[read_head++ % capacity_]; 
  }

  if (read_size < (capacity_ / 2)) {
    write_cv_.notify_all();
  }

  return n;
}

int AudioBuffer::Write(int n, float* input_left, float* input_right) {
  uint64_t write_head = bytes_written_.load(std::memory_order_acquire);
  if (write_head + n >= last_read_polled_ + capacity_) {
    last_read_polled_ = bytes_read_.load(std::memory_order_acquire);
  }

  int read_size = static_cast<int>(last_read_polled_ + capacity_ - write_head);

  n = std::min(n, read_size);
  for (int i = 0; i < n; i++) {
    buffer_r_[write_head % capacity_] = input_right[i];
    buffer_l_[write_head++ % capacity_] = input_left[i];
  }

  bytes_written_.store(write_head, std::memory_order_release);
  SeekFileToWriteHead();

  return n;
}



void AudioBuffer::WriteThreadFunc() {
  std::unique_lock<std::mutex> threadlock(write_lock_);
  uint64_t bw_local;
  // someone has to clear this to shut it up
  while (write_thread_flag_.test_and_set()) {
    bw_local = bytes_written_.load(std::memory_order_acquire);
    // buffer is full -- get up to date data
    if (bw_local == (last_read_polled_ + capacity_)) {
      last_read_polled_ = bytes_read_.load(std::memory_order_acquire);
      // after update: still full
      if (bw_local == (last_read_polled_ + capacity_)) {
        // wait until notified
        // edge cond: the buffer is killed while waiting here
        // use dtor to grab lock, clear flag, then notify, then release.
        // because the lock is grabbed: we're guaranteed to be in this state when that
        // happens, or done completely.
        write_cv_.wait(threadlock);
        continue;
      }
    }

    // once we get here: we definitely have capacity -- if WriteFromFile returns <= 0, then the file is exhausted.
    uint64_t w_len = capacity_ - (bw_local - last_read_polled_);
    // only one thread will write at a time!
    if (this->WriteFromFile(static_cast<int>(w_len)) <= 0) {
      // we obviously have capacity, so the file is exhausted or broken.
      break;
    }
  }
}

AudioBufferPacket AudioBuffer::GetBufferSpace(uint64_t n) {
  uint64_t write_head = bytes_written_.load(std::memory_order_acquire);
  if (write_head + n >= last_read_polled_ + capacity_) {
    last_read_polled_ = bytes_read_.load(std::memory_order_acquire);
  }

  uint64_t read_size = static_cast<uint64_t>(last_read_polled_ + capacity_ - write_head);

  read_size = std::min(n, read_size);

  if ((write_head % capacity_) + read_size > capacity_) {
    read_size = capacity_ - (write_head % capacity_);
  }

  float* l_offset = &buffer_l_[write_head % capacity_];
  float* r_offset = &buffer_r_[write_head % capacity_];

  bytes_written_.store(write_head + read_size, std::memory_order_release);
  return {l_offset, r_offset, read_size};
}

/**
 *  True if the thread could be spun up, false otherwise.
 */ 
bool AudioBuffer::StartWriteThread() {
  std::unique_lock<std::mutex> threadlock(write_lock_);
  if (running_) {
    return false;
  }

  write_thread_flag_.test_and_set();
  running_ = true;
  write_thread_ = std::thread(&AudioBuffer::WriteThreadFunc, this);
  write_thread_.detach();
  return true;
}

AudioBuffer::~AudioBuffer() {
  if (buffer_l_ != nullptr) {
    delete[] buffer_l_;
  }

  if (buffer_r_ != nullptr) {
    delete[] buffer_r_;
  }
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) {
  // killing write thread is the responsibility of the implementor

  if (buffer_l_ != nullptr) {
    delete[] buffer_l_;
  }

  if (buffer_r_ != nullptr) {
    delete[] buffer_r_;
  }

  // kill other's write thread
  other.DestroyWriteThread();

  // copy fields
  capacity_ = other.capacity_;
  buffer_l_ = other.buffer_l_;
  buffer_r_ = other.buffer_r_;
  other.buffer_l_ = other.buffer_r_ = nullptr;
  bytes_read_ = other.bytes_read_.load(std::memory_order_seq_cst);
  last_write_polled_ = other.last_write_polled_;
  bytes_written_ = other.bytes_written_.load(std::memory_order_seq_cst);
  last_read_polled_ = other.last_read_polled_;
  write_thread_ = std::move(other.write_thread_);
  // start up write thread again if it was already up in the moved elem
  if (other.running_) {
    StartWriteThread();
  }

  return *this;
}

AudioBuffer::AudioBuffer(AudioBuffer&& other) : capacity_(other.capacity_) {
  // copy fields
  other.DestroyWriteThread();
  buffer_l_ = other.buffer_l_;
  buffer_r_ = other.buffer_r_;
  other.buffer_l_ = other.buffer_r_ = nullptr;
  bytes_read_ = other.bytes_read_.load(std::memory_order_seq_cst);
  last_write_polled_ = other.last_write_polled_;
  bytes_written_ = other.bytes_written_.load(std::memory_order_seq_cst);
  last_read_polled_ = other.last_read_polled_;
  write_thread_ = std::move(other.write_thread_);
  // start up write thread again if it was already up in the moved elem
  if (other.running_) {
    StartWriteThread();
  }
}

void AudioBuffer::DestroyWriteThread() {
  if (running_) {
    std::unique_lock<std::mutex> moved_thread_lock(write_lock_);
    write_thread_flag_.clear();
    write_cv_.notify_all();
    moved_thread_lock.unlock();
    running_ = false;
  }
}

}
}