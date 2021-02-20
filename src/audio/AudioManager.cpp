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

  PaStreamParameters* out = new PaStreamParameters();
  out->channelCount = 2;
  out->device = Pa_GetDefaultOutputDevice();
  out->sampleFormat = paFloat32;
  out->suggestedLatency = 0.05;

  err = Pa_OpenStream(&stream_, NULL, out, SAMPLE_RATE, paFramesPerBufferUnspecified, paNoFlag, &AudioManager::CallbackFunc, this);
  if (err != paNoError) {
    BOOST_LOG_TRIVIAL(error) << "Could not initialize PortAudio: " << Pa_GetErrorText(err);
    Pa_Terminate();
    throw PortAudioException("Could not initialize PA");
  }

  Pa_StartStream(stream_);

  // start the buffer creation thread
  buffer_thread_flag_.test_and_set();
  buffer_creation_thread_ = std::thread(&AudioManager::QueueThreadfunc, this);
}

int AudioManager::AddFileToBuffer(const std::string& filename, AudioFiletype file_type) {
  int index = -1;
  buffer_info* info;
  buffer_status desired_value = AVAILABLE;

  { // wraps buffer lock usage
    std::unique_lock<std::mutex> buffer_lock(buffer_info_write_lock_);
    for (int i = 0; i < AUDIO_MGR_MAX_BUFFER_COUNT; i++) {
      info = &buffers_[i];
      if (info->status.compare_exchange_weak(desired_value, ALLOCATING)) {
        index = i;
        break;
      }
    }
  }

  if (index == -1) {
    return -1;
    // could not allocate space
  }

  {
    std::unique_lock<std::mutex> queue_lock(buffer_queue_lock_);
    buffer_creation_queue_.push({filename, file_type, index});
  }

  buffer_thread_cv_.notify_all();
  return index;
}

void AudioManager::QueueThreadfunc() {
  queue_info info_queue;
  while (buffer_thread_flag_.test_and_set()) {
    BOOST_LOG_TRIVIAL(trace) << "queue threadfunc runs once :)";
    { // fetch entry from queue and place in info
      std::unique_lock<std::mutex> buffer_queue_lock(buffer_queue_lock_);
      while (buffer_creation_queue_.empty()) {
        // looping is beneficial for wait -- check in here if the flag's been cleared, then return if so
        if (!buffer_thread_flag_.test_and_set()) {
          return;
        }

        BOOST_LOG_TRIVIAL(trace) << "no new samples -- waiting...";
        buffer_thread_cv_.wait(buffer_queue_lock);
      }

      info_queue = buffer_creation_queue_.front();
      buffer_creation_queue_.pop();
    }

    buffer_info* info_buffer;

    info_buffer = &buffers_[info_queue.index];

    switch (info_queue.type) {
      case OGG:
        if (info_buffer->buffer != nullptr) {
          delete info_buffer->buffer;
        }

        info_buffer->buffer = new AudioBufferOgg(4096, info_queue.filename);
        info_buffer->status = USED;
        info_buffer->buffer->StartWriteThread();
        break;
      default:
        BOOST_LOG_TRIVIAL(error) << "Unknown buffer type received -- " << info_queue.type;
    }
  }
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
  for (unsigned int i = 0; i < frameCount; i++) {
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

  buffer_thread_flag_.clear();
  buffer_thread_cv_.notify_all();
  buffer_creation_thread_.join();

  for (int i = 0; i < AUDIO_MGR_MAX_BUFFER_COUNT; i++) {
    if (buffers_[i].buffer != nullptr) {
      delete buffers_[i].buffer;
    }
  }
}

}
}