#include <audio/AudioBuffer.hpp>
#include <audio/AudioBufferOgg.hpp>

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace audio {

AudioBufferOgg::AudioBufferOgg(int capacity, const std::string& filename) : AudioBuffer(capacity) {
  int err;
  vorbis_file_ = stb_vorbis_open_filename(filename.c_str(), &err, NULL);
  if (vorbis_file_ == NULL) {
    BOOST_LOG_TRIVIAL(error) << "Vorbis open failed with err code " << err;
  }

  eof_ = false;
}

int AudioBufferOgg::WriteFromFile(int n) {
  if (vorbis_file_ == nullptr) {

    return -1;
  }

  // attempt to write 0 bytes -- let's just bounce it back.
  if (n == 0) {
    return 0;
  }
  uint64_t write_head = bytes_written_.load(std::memory_order_acquire);
  if (write_head + n >= last_read_polled_ + capacity_) {
    last_read_polled_ = bytes_read_.load(std::memory_order_acquire);
  }


  float* buffers_[2] = {&buffer_l_[write_head % capacity_], &buffer_r_[write_head % capacity_]};
  // if we're splitting our buffer: we need to do a pair of writes, instead of just one!
  n = std::min(n, static_cast<int>(last_read_polled_ + capacity_ - write_head));
  int samples_written = 0;
  if ((write_head % capacity_) + n > capacity_) {
    int first_write_size = (capacity_ - (write_head % capacity_));
    samples_written = stb_vorbis_get_samples_float(vorbis_file_,
                                                   2,
                                                   buffers_,
                                                   first_write_size);
    
    // todo: factor this out lol
    if (samples_written == 0) {
      eof_.store(true);
      bytes_written_.fetch_add(samples_written, std::memory_order_release);
      return samples_written;
    }

    // point back to start of buffers
    buffers_[0] = buffer_l_;
    buffers_[1] = buffer_r_;

    // decrement the samples we've already written
    n -= samples_written;
  }

  samples_written += stb_vorbis_get_samples_float(vorbis_file_,
                                                     2,
                                                     buffers_,
                                                     n);
  if (samples_written == 0) {
    // our file buffer is exhausted!
    // how can we communicate this to the reader?
    // use an EOF flag
    // if true: file is exhausted
    eof_.store(true);
  }

  bytes_written_.fetch_add(samples_written, std::memory_order_release);
  return samples_written;
}

bool AudioBufferOgg::EndOfFile() {
  return eof_.load();
}

AudioBufferOgg::~AudioBufferOgg() {
  // for concurrency reasons: we have to clean up the write thread ourselves
  // it shouldn't matter in the long run though lol
  if (running_) {
    std::unique_lock<std::mutex> thread_lock(write_lock_);
    write_thread_flag_.clear();
    write_cv_.notify_all();
    write_thread_.join();
    // should terminate shortly after this
  }
  

  if (vorbis_file_ != nullptr) {
    stb_vorbis_close(vorbis_file_);
  }
}

AudioBufferOgg& AudioBufferOgg::operator=(AudioBufferOgg&& other) {
  AudioBuffer::operator=(std::move(other));
  this->vorbis_file_ = other.vorbis_file_;
  other.vorbis_file_ = nullptr;
  return *this;
}

AudioBufferOgg::AudioBufferOgg(AudioBufferOgg&& other) : AudioBuffer(dynamic_cast<AudioBuffer&&>(other)) {
  this->vorbis_file_ = other.vorbis_file_;
  other.vorbis_file_ = nullptr;
}

}
}