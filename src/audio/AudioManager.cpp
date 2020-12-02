#include <audio/AudioManager.hpp>
#include <audio/AudioBufferOgg.hpp>

#include <audio/exception/PortAudioException.hpp>

#include <boost/log/trivial.hpp>

#define SAMPLE_RATE 44100

namespace monkeysworld {
namespace audio {

using exception::PortAudioException;

AudioManager::AudioManager() {
  int err = Pa_Initialize();
  if (err != paNoError) {

    BOOST_LOG_TRIVIAL(error) << "Could not initialize PortAudio: " << Pa_GetErrorText(err);
    throw PortAudioException("Could not initialize PortAudio");
  }

  for (int i = 0; i < AUDIO_MGR_MAX_BUFFER_COUNT; i++) {
    buffers_[i].status = AVAILABLE;
    buffers_[i].buffer = nullptr;
  }

  err = Pa_OpenDefaultStream(&stream_, 0, 2, paFloat32, SAMPLE_RATE, 0, &AudioManager::CallbackFunc, this);
  if (err != paNoError) {
    BOOST_LOG_TRIVIAL(error) << "Could not initialize PortAudio: " << Pa_GetErrorText(err);
    Pa_Terminate();
    throw PortAudioException("Could not initialize PA");
  }

  Pa_StartStream(stream_);
}

int AudioManager::AddFileToBuffer(const std::string& filename, AudioFiletype file_type) {
  AudioBuffer* new_buffer;
  buffer_info* info;
  

  std::unique_lock<std::mutex> templock(buffer_info_write_lock_);
  for (int i = 0; i < AUDIO_MGR_MAX_BUFFER_COUNT; i++) {
    info = &buffers_[i];
    if (info->status == AVAILABLE) {
      info->status = ALLOCATING;
      templock.unlock();
      if (info->buffer != nullptr) {
        delete info->buffer;
      }

      switch (file_type) {
        case OGG:
          // i could try creating this on a new thread :)
          // TODO: Create a new thread which can take care of buffer creation
          // provide a struct for passing information to a thread
          // then let it create the buffers in its little world
          // instead of forcing me to deal with it
          new_buffer = new AudioBufferOgg(16384, filename);
          info->buffer = new_buffer;
          info->status = USED;
          info->buffer->StartWriteThread();
          return i;
          break;
        default:
          BOOST_LOG_TRIVIAL(error) << "Could not add file to buffer -- filetype == " << file_type;
          return -1;
      }
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
  switch (info->status) {
    case AVAILABLE:
      if (info->buffer != nullptr) {
        delete info->buffer;
        info->buffer = nullptr;
      }
      break;
    case USED:
      info->status = DELETING;
      break;
  }
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
  // cannot throw exceptions in dtor :)
  int err = Pa_CloseStream(stream_);
  if (err != paNoError) {
    BOOST_LOG_TRIVIAL(error) << "Could not close PA stream for audio manager";
  }

  err = Pa_Terminate();
  if (err != paNoError) {
    BOOST_LOG_TRIVIAL(error) << "Failed to terminate PA";
  }
  for (int i = 0; i < AUDIO_MGR_MAX_BUFFER_COUNT; i++) {
    if (buffers_[i].buffer != nullptr) {
      delete buffers_[i].buffer;
    }
  }
}

}
}