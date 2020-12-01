#include <audio/AudioManager.hpp>
#include <audio/AudioBufferOgg.hpp>

#include <audio/exception/PortAudioException.hpp>

#include <boost/log/trivial.hpp>

#define SAMPLE_RATE 48000

namespace monkeysworld {
namespace audio {

using exception::PortAudioException;

// do this last
AudioManager::AudioManager() {
  int err = Pa_Initialize();
  if (err != paNoError) {

    BOOST_LOG_TRIVIAL(error) << "Could not initialize PortAudio: " << Pa_GetErrorText(err);
    throw PortAudioException("Could not initialize PortAudio");
  }

  for (int i = 0; i < AUDIO_MGR_MAX_BUFFER_COUNT; i++) {
    buffers_[i].status = AVAILABLE;
  }

  Pa_OpenDefaultStream(&stream_, 0, 2, paFloat32, SAMPLE_RATE, 0, &AudioManager::CallbackFunc, this);
}

int AudioManager::AddFileToBuffer(const std::string& filename, AudioFiletype file_type) {
  AudioBuffer* new_buffer;
  

  std::unique_lock<std::mutex>(buffer_info_write_lock_);
  for (int i = 0; i < AUDIO_MGR_MAX_BUFFER_COUNT; i++) {
    if (buffers_[i].status == AVAILABLE) {
      if (buffers_[i].buffer != nullptr) {
        delete buffers_[i].buffer;
      }

      switch (file_type) {
        case OGG:
          new_buffer = new AudioBufferOgg(16384, filename);
        default:
          return -1;
      }

      buffers_[i].buffer = new_buffer;
      buffers_[i].status = USED;
      buffers_[i].buffer->StartWriteThread();
      return i;
    }
  }

  return -1;
}

int AudioManager::RemoveFileFromBuffer(int stream) {
  if (stream > AUDIO_MGR_MAX_BUFFER_COUNT || stream < 0) {
    // invalid marker
    return -1;
  }

  buffer_info* info = &buffers_[stream];
  std::unique_lock<std::mutex>(buffer_info_write_lock_);
  if (info->status != USED) {
    // buffer is already cleaned up, or is about to be cleaned up.
    return 0;
  }

  // the client could delete it between now and then - but its no big deal! lol
  info->status = DELETING;
  return 0;
}

int AudioManager::CallbackFunc(const void* input,
                                void* output,
                                unsigned long frameCount,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void* userData) {


  float* output_buffer = reinterpret_cast<float*>(output);
  AudioManager* mgr = reinterpret_cast<AudioManager*>(userData);
  buffer_info* info;
  int samples_read;
  for (int i = 0; i < frameCount; i++) {
    // zero the output buffer
    output_buffer[2 * i] = output_buffer[2 * i + 1] = 0.0f;
  }

  for (int i = 0; i < AUDIO_MGR_MAX_BUFFER_COUNT; i++) {
    info = &mgr->buffers_[i];
    switch (info->status) {
      case DELETING:
        info->status = AVAILABLE;
      case AVAILABLE:
        break;
      case USED:
        // essentially reads zeroes if the sample cannot be fetched :)
        samples_read = info->buffer->ReadAddInterleaved(frameCount, output_buffer);
        if (samples_read == 0 && info->buffer->EndOfFile()) {
          info->status = AVAILABLE;
        }
      default:
        break;
    }
  }

  return paContinue;
}

AudioManager::~AudioManager() {
  int err = Pa_CloseStream(stream_);
  if (err != paNoError) {
    BOOST_LOG_TRIVIAL(error) << "Could not close PA stream for audio manager";
    throw PortAudioException("Could not close PA stream for audio manager");
  }

  err = Pa_Terminate();
  if (err != paNoError) {
    BOOST_LOG_TRIVIAL(error) << "Failed to terminate PA";
    throw PortAudioException("Failed to terminate PA");
  }
  for (int i = 0; i < AUDIO_MGR_MAX_BUFFER_COUNT; i++) {
    if (buffers_[i].buffer != nullptr) {
      delete buffers_[i].buffer;
    }
  }
}

}
}