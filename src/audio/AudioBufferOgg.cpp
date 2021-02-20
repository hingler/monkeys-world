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
  file_path_ = filename;
  vorbis_file_ = nullptr;
}

void AudioBufferOgg::OpenFile() {
  int err;
  int memsize = ALLOC_INC;
  vorbis_buf_.alloc_buffer = nullptr;

  do {
    BOOST_LOG_TRIVIAL(trace) << "open " << file_path_ << " -- trying " << memsize << " bytes of mem";
    if (vorbis_buf_.alloc_buffer != nullptr) {
      delete[] vorbis_buf_.alloc_buffer;
    }

    vorbis_buf_.alloc_buffer = new char[memsize];
    vorbis_buf_.alloc_buffer_length_in_bytes = memsize;
    vorbis_file_ = stb_vorbis_open_filename(file_path_.c_str(), &err, &vorbis_buf_);
    memsize += ALLOC_INC;
  } while (vorbis_file_ == NULL && err == STBVorbisError::VORBIS_outofmem && memsize <= ALLOC_MAX);

  if (vorbis_file_ == NULL) {
    // some other error occurred
    BOOST_LOG_TRIVIAL(error) << "Vorbis open failed with err code " << err;
    // throw an exception here
  }

  info_ = stb_vorbis_get_info(vorbis_file_);
  eof_ = false;
}

int AudioBufferOgg::WriteFromFile(int n) {
  if (vorbis_file_ == nullptr) {
    OpenFile();
  }

  // attempt to write 0 bytes -- let's just bounce it back.
  if (n == 0) {
    return 0;
  }

  int readsize = n;
  while (readsize > 0) {
    // how do we tell the thing that we don't want all that space?
    // figure it out later lol
    AudioBufferPacket packet = GetBufferSpace(readsize);
    float* buffers_[2] = {packet.left, packet.right};
    int samples_written = stb_vorbis_get_samples_float(vorbis_file_,
                                                       2,
                                                       buffers_,
                                                       static_cast<int>(packet.capacity));
    if (samples_written == 0) {
      eof_.store(true);
      return n - readsize;
    }

    if (info_.channels == 1) {
      for (int i = 0; i < samples_written; i++) {
        buffers_[1][i] = buffers_[0][i];
      }
    }

    readsize -= samples_written;
  }

  return n;
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
    int seek_res = stb_vorbis_seek(vorbis_file_, static_cast<unsigned int>(bytes_written_.load(std::memory_order_acquire)));
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