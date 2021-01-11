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

// TODO: its not necessary at this point, but turning cachedloader into a template interface
//       might be nice:)
class TextureLoader : public CachedLoader {
 public:
  TextureLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
                std::vector<cache_record> cache);

  std::shared_ptr<shader::Texture> LoadTexture(const std::string& path);

  std::future<std::shared_ptr<shader::Texture>> LoadTextureAsync(const std::string& path);

  std::vector<cache_record> GetCache() override;

  loader_progress GetLoaderProgress() override;
  
  void WaitUntilLoaded() override;
 
 private:
  std::shared_ptr<shader::Texture> LoadTextureFromFile(const std::string& path);

  void LoadTextureToCache(const cache_record& record);

  loader_progress loader_;
  std::mutex loader_mutex_;
  std::shared_timed_mutex cache_mutex_;
  std::unordered_map<std::string, std::shared_ptr<shader::Texture>> texture_cache_;
  std::shared_ptr<LoaderThreadPool> thread_pool_;
  std::condition_variable load_cond_var_;
  
};

}
}

#endif