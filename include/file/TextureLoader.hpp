#ifndef TEXTURE_LOADER_H_
#define TEXTURE_LOADER_H_

#include <file/CachedLoader.hpp>
#include <file/LoaderThreadPool.hpp>

#include <shader/Texture.hpp>

#include <condition_variable>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace monkeysworld {
namespace file {

class TextureLoader : public CachedLoader<std::shared_ptr<shader::Texture>, TextureLoader> {
  friend class CachedLoader<std::shared_ptr<shader::Texture>, TextureLoader>;
 public:
  TextureLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
                std::vector<cache_record> cache);

  std::vector<cache_record> GetCache() override;

  loader_progress GetLoaderProgress() override;
  
  void WaitUntilLoaded() override;
 
 protected:
  std::shared_ptr<shader::Texture> LoadFromFile(const std::string& path);
 
 private:

  void LoadTextureToCache(const cache_record& record);

  loader_progress loader_;
  std::mutex loader_mutex_;
  std::shared_timed_mutex cache_mutex_;
  std::unordered_map<std::string, std::shared_ptr<shader::Texture>> texture_cache_;
  std::condition_variable load_cond_var_;
  
};

}
}

#endif