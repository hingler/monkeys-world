#include <audio/AudioManager.hpp>

#include <audio/AudioBufferOgg.hpp>

namespace monkeysworld {
namespace audio {

// do this last
AudioManager::AudioManager() {

}

int AudioManager::AddFileToBuffer(const std::string& filename, AudioFiletype file_type) {
  AudioBuffer* new_buffer;
  switch (file_type) {
    case OGG:
      new_buffer = new AudioBufferOgg(16384, filename);
    default:
      new_buffer = nullptr;
  } 

  if (new_buffer = nullptr) {
    return -1;
  }

  std::unique_lock<std::mutex>(buffer_info_write_lock_);
  for (int i = 0; i < AUDIO_MGR_MAX_BUFFER_COUNT; i++) {
    if (buffers_[i].status == AVAILABLE) {
      if (buffers_[i].buffer != nullptr) {
        delete buffers_[i].buffer;
      }

      buffers_[i].buffer = new_buffer;
      buffers_[i].status = USED;
      buffers_[i].buffer->StartWriteThread();
      return i;
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
  if (info->status != USED) {
    // buffer is already cleaned up, or is about to be cleaned up.
    return 0;
  }

  // the client could delete it between now and then - but its no big deal! lol
  info->status = DELETING;
}

}
}