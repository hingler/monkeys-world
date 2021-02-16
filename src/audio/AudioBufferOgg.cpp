#include <audio/AudioBuffer.hpp>
#include <audio/AudioBufferOgg.hpp>

#include <boost/log/trivial.hpp>

// 32Kb borrowed from https://github.com/nothings/stb/issues/114
#define ALLOC_INC 32768

// 512 kb alloc max (i think suggested max is like 250 or so)
#define ALLOC_MAX (ALLOC_INC * 16)

namespace monkeysworld {
namespace audio {

AudioBufferOgg::AudioBufferOgg(int capacity, const std::string& filename) : AudioBuffer(capacity) {

  int err;
  int memsize = ALLOC_INC;
  vorbis_buf_.alloc_buffer = nullptr;

  do {
    BOOST_LOG_TRIVIAL(trace) << "open " << filename << " -- trying " << memsize << " bytes of mem";
    if (vorbis_buf_.alloc_buffer != nullptr) {
      delete[] vorbis_buf_.alloc_buffer;
    }

    vorbis_buf_.alloc_buffer = new char[memsize];
    vorbis_buf_.alloc_buffer_length_in_bytes = memsize;
    vorbis_file_ = stb_vorbis_open_filename(filename.c_str(), &err, &vorbis_buf_);
    memsize += ALLOC_INC;
  } while (vorbis_file_ == NULL && err == STBVorbisError::VORBIS_outofmem && memsize <= ALLOC_MAX);

  if (vorbis_file_ == NULL) {
    // some other error occurred
    BOOST_LOG_TRIVIAL(error) << "Vorbis open failed with err code " << err;
  }

  info_ = stb_vorbis_get_info(vorbis_file_);
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

    if (info_.channels == 1) {
      // copy data over if in mono
      for (int i = 0; i < first_write_size; i++) {
        buffers_[1][i] = buffers_[0][i];
      }
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

  if (info_.channels == 1) {
    // copy data over if in mono
    for (int i = 0; i < n; i++) {
      buffers_[1][i] = buffers_[0][i];
    }
  }

  bytes_written_.fetch_add(samples_written, std::memory_order_release);
  return samples_written;
}

bool AudioBufferOgg::EndOfFile() {
  return eof_.load();
}

void AudioBufferOgg::SeekFileToWriteHead() {
  int sample_count = stb_vorbis_stream_length_in_samples(vorbis_file_);
  uint64_t write_head = bytes_written_.load(std::memory_order_acquire);
  if (write_head >= sample_count) {
    stb_vorbis_seek(vorbis_file_, sample_count);
    // is this necessary?
    eof_ = true;
  } else {
    int seek_res = stb_vorbis_seek(vorbis_file_, bytes_written_.load(std::memory_order_acquire));
    if (seek_res == 0) {
      // some other error occured!
      BOOST_LOG_TRIVIAL(error) << "Seek on vorbis file failed with error " << stb_vorbis_get_error(vorbis_file_);
    }
  }
}

AudioBufferOgg::~AudioBufferOgg() {
  // for concurrency reasons: we have to clean up the write thread ourselves
  // it shouldn't matter in the long run though
  if (running_) {
    std::unique_lock<std::mutex> thread_lock(write_lock_);
    write_thread_flag_.clear();
    write_cv_.notify_all();
    // should terminate shortly after this
  }
  

  if (vorbis_file_ != nullptr) {
    stb_vorbis_close(vorbis_file_);
  }

  delete[] vorbis_buf_.alloc_buffer;
}

AudioBufferOgg& AudioBufferOgg::operator=(AudioBufferOgg&& other) {
  DestroyWriteThread();
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