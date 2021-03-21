#ifndef AUDIO_LOADER_H_
#define AUDIO_LOADER_H_

#include <file/CachedLoader.hpp>
#include <file/LoaderThreadPool.hpp>

#include <audio/AudioBuffer.hpp>

#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>

namespace monkeysworld {
namespace file {

// return shared ptrs to audio buffer

class AudioLoader : public CachedLoader<std::shared_ptr<audio::AudioBuffer>, AudioLoader> {
 public:
  AudioLoader(std::shared_ptr<LoaderThreadPool> thread_pool, std::vector<cache_record> cache);

  std::vector<cache_record> GetCache() override;
  loader_progress GetLoaderProgress() override;
  void WaitUntilLoaded() override;

  /**
   *  Loads the audio file.
   *  @param path - the path to our desired audio file.
   */ 
  std::shared_ptr<audio::AudioBuffer> LoadFile(const std::string& path);

  bool IsCached(const std::string& path) override;

  ~AudioLoader();
 private:

  struct AudioCache {
    float* left;      // l data
    float* right;     // r data
    int sample_count; // num samples
  };

  std::shared_ptr<audio::AudioBuffer> LoadFromPath(const std::string& path);
  
  /**
   *  Loads samples to the cache.
   */ 
  void LoadFileToCache(cache_record& record);
  loader_progress loader_;
  std::mutex loader_mutex_;
  std::condition_variable load_cond_var_;

  std::mutex cache_mutex_;
  std::unordered_map<std::string, AudioCache> cache_;
};

}
}

#endif