#include <audio/AudioBuffer.hpp>

namespace monkeysworld {
namespace audio {

AudioBuffer::AudioBuffer(int capacity) : capacity_(capacity) {
  buffer_l_ = new float[capacity];
  buffer_r_ = new float[capacity];
  bytes_read_ = 0;
  bytes_written_ = 0;
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

int AudioBuffer::Peek(int n, float* output_left, float* output_right) {
  uint64_t read_head = bytes_read_.load(std::memory_order_acquire);
  if (read_head + n >= last_write_polled_) {
    last_write_polled_ = bytes_written_.load(std::memory_order_acquire);
  }

  // number of samples which we can still read
  int read_size = static_cast<int>(last_write_polled_ - read_head);

  read_size = std::min(n, read_size);
  for (int i = 0; i < read_size; i++) {
    output_right[i] = buffer_r_[read_head % capacity_];
    output_left[i] = buffer_l_[read_head++ % capacity_]; 
  }

  if (read_size - n < (capacity_ / 2)) {
    write_cv_.notify_all();
  }

  return read_size;
}

int AudioBuffer::Write(int n, float* input_left, float* input_right) {
  uint64_t write_head = bytes_written_.load(std::memory_order_acquire);
  if (write_head + n >= last_read_polled_ + capacity_) {
    last_read_polled_ = bytes_read_.load(std::memory_order_acquire);
  }

  n = std::min(n, static_cast<int>(last_read_polled_ + capacity_ - write_head));
  for (int i = 0; i < n; i++) {
    buffer_r_[write_head % capacity_] = input_right[i];
    buffer_l_[write_head++ % capacity_] = input_left[i];
  }

  bytes_written_.store(write_head, std::memory_order_release);
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
    if (this->WriteFromFile(w_len) <= 0) {
      // we obviously have capacity, so the file is exhausted or broken.
      break;
    }
  }
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
  write_thread_ = std::thread(&AudioBuffer::WriteThreadFunc, this);
  return true;
}

AudioBuffer::~AudioBuffer() {
  std::unique_lock<std::mutex> thread_lock(write_lock_);
  write_thread_flag_.clear();
  write_cv_.notify_all();
  thread_lock.unlock();
  // should terminate shortly after this
  if (running_) {
    write_thread_.join();
  }

  if (buffer_l_ != nullptr) {
    delete[] buffer_l_;
  }

  if (buffer_r_ != nullptr) {
    delete[] buffer_r_;
  }
}

AudioBuffer& AudioBuffer::operator=(AudioBuffer&& other) {
  // kill my write thread
  if (running_) {
    std::unique_lock<std::mutex> thread_lock(write_lock_);
    write_thread_flag_.clear();
    write_cv_.notify_all();
    thread_lock.unlock();
    write_thread_.join();
  }

  if (buffer_l_ != nullptr) {
    delete[] buffer_l_;
  }

  if (buffer_r_ != nullptr) {
    delete[] buffer_r_;
  }

  // kill other's write thread
  if (other.running_) {
    std::unique_lock<std::mutex> moved_thread_lock(write_lock_);
    other.write_thread_flag_.clear();
    other.write_cv_.notify_all();
    moved_thread_lock.unlock();
    other.write_thread_.join();
  }

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
  // kill other's write thread
  if (other.running_) {
    std::unique_lock<std::mutex> moved_thread_lock(write_lock_);
    other.write_thread_flag_.clear();
    other.write_cv_.notify_all();
    moved_thread_lock.unlock();
    other.write_thread_.join();
  }

  // copy fields
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

}
}