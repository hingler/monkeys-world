#ifndef FILE_LOADER_H_
#define FILE_LOADER_H_

#include <file/CachedLoader.hpp>
#include <file/LoaderThreadPool.hpp>
#include <file/CacheStreambuf.hpp>

#include <condition_variable>
#include <future>
#include <mutex>
#include <unordered_map>
#include <shared_mutex>

namespace monkeysworld {
namespace file {

/**
 *  Cached loader for full file content.
 *  Caches the original content of the file for clients to modify as needed.
 */ 
class FileLoader : public CachedLoader<CacheStreambuf, FileLoader> {
 public:
  FileLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
             std::vector<cache_record> cache);

  CacheStreambuf LoadFile(const std::string& path);
  std::vector<cache_record> GetCache() override;
  loader_progress GetLoaderProgress() override;
  void WaitUntilLoaded() override;

  bool IsCached(const std::string& path) override;
 
 private:
  // loads a file into an std::vector, puts that vector in the cache,
  // then returns a CacheStreambuf which reads from that vector.

  // method to handle cache loading
  void LoadFileToCache(cache_record& record);

  loader_progress loader_;
  std::mutex loader_mutex_;
  std::shared_timed_mutex cache_mutex_;
  std::unordered_map<std::string, std::shared_ptr<std::vector<char>>> file_cache_;
  std::condition_variable load_cond_var_;
};

}
}

#endif