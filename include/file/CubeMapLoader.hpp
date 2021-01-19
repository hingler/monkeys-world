#ifndef CUBE_MAP_LOADER_H_
#define CUBE_MAP_LOADER_H_

#include <file/CachedLoader.hpp>
#include <file/LoaderThreadPool.hpp>

#include <shader/CubeMap.hpp>

#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace monkeysworld {
namespace file {

class CubeMapLoader : public CachedLoader<std::shared_ptr<shader::CubeMap>, CubeMapLoader> {
 public:
  CubeMapLoader(std::shared_ptr<LoaderThreadPool> thread_pool, std::vector<cache_record> cache);

  std::vector<cache_record> GetCache() override;
  loader_progress GetLoaderProgress() override;
  void WaitUntilLoaded() override;
  
  /**
   *  @param path - a comma-separated, concatenated list of six files (TODO: for now...)
   */ 
  std::shared_ptr<shader::CubeMap> LoadFile(const std::string& path);

 protected:
 private:
  void LoadFileToCache(cache_record& record);

  loader_progress loader_;
  std::mutex loader_mutex_;
  std::shared_timed_mutex cache_mutex_;
  std::unordered_map<std::string, std::shared_ptr<shader::CubeMap>> file_cache_;
  std::condition_variable load_cond_var_;
};

}
}

#endif