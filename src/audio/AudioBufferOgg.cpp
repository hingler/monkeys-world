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

  if (eof_.load()) {
    return 0;
  }

  int readsize = n;
  do {
    // how do we tell the thing that we don't want all that space?
    // figure it out later lol
    AudioBufferPacket packet = GetBufferSpace(readsize);
    float* buffers_[2] = {packet.left, packet.right};
    // as far as i can tell: samples_written != packet.capacity only if we are at EOF.
    // if not: we can invent some way to keep the packet around until it is exhausted.
    int samples_written = stb_vorbis_get_samples_float(vorbis_file_,
                                                       2,
                                                       buffers_,
                                                       static_cast<int>(packet.capacity));
    
    IncrementWriteHead(samples_written);

    if (packet.capacity == 0) {
      // we're all out of space!!!
      // cut our losses and return what we could read
      return n - readsize;
    }
    
    if (samples_written < packet.capacity) {
      for (int i = samples_written; i < packet.capacity; i++) {
        // if we EOF, ensure the rest of the packet is given 0's, to avoid audio glitches
        // due to playing back old samples.
        buffers_[0][i] = buffers_[1][i] = 0.0f;
      }

      if (IsLooped()) {
        // reset the file head
        stb_vorbis_seek(vorbis_file_, 0);
      } else {


        eof_.store(true);
        return n - readsize;
      }
    }

    if (info_.channels == 1) {
      for (int i = 0; i < samples_written; i++) {
        buffers_[1][i] = buffers_[0][i];
      }
    }

    readsize -= samples_written;
  } while (readsize > 0);


  return n;
}

bool AudioBufferOgg::EndOfFile() {
  return eof_.load();
}

void AudioBufferOgg::SeekFileToWriteHead() {
  if (vorbis_file_ == nullptr) {
    OpenFile();
  }
  int sample_count = stb_vorbis_stream_length_in_samples(vorbis_file_);
  uint64_t write_head = GetBytesWritten();
  if (write_head >= sample_count) {
    // handle loops
    if (IsLooped()) {
      // modulo and seek to offset
      uint64_t sample_effective = write_head % static_cast<uint64_t>(sample_count);
      stb_vorbis_seek(vorbis_file_, static_cast<unsigned int>(sample_effective));
    } else {
      stb_vorbis_seek(vorbis_file_, sample_count);
      // is this necessary?
      eof_ = true;
    }
  } else {
    int seek_res = stb_vorbis_seek(vorbis_file_, static_cast<unsigned int>(GetBytesWritten()));
    if (seek_res == 0) {
      // some other error occured!
      BOOST_LOG_TRIVIAL(error) << "Seek on vorbis file failed with error " << stb_vorbis_get_error(vorbis_file_);
    }
  }
}

AudioBufferOgg::~AudioBufferOgg() {
  DestroyWriteThread();
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