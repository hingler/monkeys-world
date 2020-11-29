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
}

int AudioBufferOgg::WriteFromFile(int n) {
  if (vorbis_file_ == nullptr) {
    return -1;
  }

  uint64_t write_head = bytes_written_.load(std::memory_order_acquire);
  if (write_head + n >= last_read_polled_ + capacity_) {
    last_read_polled_ = bytes_read_.load(std::memory_order_acquire);
  }

  float* buffers_[2] = {buffer_l_, buffer_r_};

  n = std::min(n, static_cast<int>(last_read_polled_ + capacity_ - write_head));
  int samples_written = stb_vorbis_get_samples_float(vorbis_file_,
                                                     2,
                                                     buffers_,
                                                     n);
  bytes_written_.fetch_add(n, std::memory_order_release);
  return n;
}

AudioBufferOgg::~AudioBufferOgg() {
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