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
class FileLoader : public CachedLoader {
 public:
  FileLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
             std::vector<cache_record> cache);

  /**
   *  Synchronously load a file
   *  @param path - the path to the file
   *  @returns a CacheStreambuf corresponding with the file.
   *           if the path is invalid, CacheStreambuf::valid will return false.
   */ 
  CacheStreambuf LoadFile(const std::string& path);

  /**
   *  Asynchronously load a file
   *  @param path - the path to the file
   *  @returns a future which eventually resolves to a cache streambuf.
   *           CacheStreambuf::valid will return false if the file is invalid.
   */ 
  std::future<CacheStreambuf> LoadFileAsync(const std::string& path);

  std::vector<cache_record> GetCache() override;
  loader_progress GetLoaderProgress() override;
  void WaitUntilLoaded() override;
 private:
  // loads a file into an std::vector and returns the result
  std::shared_ptr<std::vector<char>> LoadFileAsVector(const std::string& path);

  // method to handle cache loading
  void LoadFileToCache(cache_record& record);

  loader_progress loader_;
  std::mutex loader_mutex_;
  std::shared_timed_mutex cache_mutex_;
  std::unordered_map<std::string, std::shared_ptr<std::vector<char>>> file_cache_;
  std::shared_ptr<LoaderThreadPool> thread_pool_;
  std::condition_variable load_cond_var_;
};

}
}

#endif