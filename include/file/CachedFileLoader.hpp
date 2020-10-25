#ifndef FILE_LOADER_H_
#define FILE_LOADER_H_

#include <file/CacheStreambuf.hpp>

#include <atomic>
#include <cinttypes>
#include <fstream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace screenspacemanager {
namespace file {

/**
 *  Provides information on loader progress.
 */ 
struct loader_progress {
  size_t bytes_read;          // number of bytes read thus far
  size_t bytes_sum;           // number of bytes expected
};

/**
 *  Internally stores objects contained by the cache.
 */ 
struct loader_record {
  size_t file_size;                             // size of an expected file
  std::shared_ptr<std::vector<char>> data;      // data associated with the file
};

/**
 *  The CachedFileLoader is designed to streamline the process of loading files,
 *  and displaying loading status.
 * 
 *  On first load: a cache is generated which records all files accessed.
 *  On successive loads: the cache is read, and the files contained in it are loaded automatically.
 *  Cache state may be changed. The cache will always grow but never shrink, just in case.
 * 
 *  To clear the cache: just delete the respective cache file.
 */ 
class CachedFileLoader {
 public:

  static const uint32_t CACHE_MAGIC = 0x4D534657;   // WFSM
  static const uint64_t CACHE_DATA_START = 12;

  /**
   *  default ctor for CachedFileLoader.
   *  No set up in here.
   */ 
  CachedFileLoader();

  /**
   *  Constructs a new CachedFileLoader.
   *  The CachedFileLoader will use the cache file located in resources/cache/<cache_name>.filecache.
   *  This ctor call will also spin up the load thread.
   */ 
  CachedFileLoader(const std::string& cache_name);

  /**
   *  Returns the current state of the loader, in terms of bytes loaded vs. bytes expected.
   *  @return copy of the current loader progress.
   */ 
  loader_progress GetLoaderProgress();

  /**
   *  Spins until cache loading is complete.
   */ 
  void SpinUntilCached();

  /**
   *  Returns a stream buffer corresponding with the desired file.
   *  Updates internal cache if necessary.
   *  @param path - the path to the desired file.
   *  If the file has not been loaded, spins until the file has been cached, then returns.
   */ 
  std::unique_ptr<CacheStreambuf> LoadFile(const std::string& path);

  // TODO: hangs on load
  ~CachedFileLoader();
  CachedFileLoader(const CachedFileLoader& other) = delete;
  CachedFileLoader(CachedFileLoader&& other);
  CachedFileLoader& operator=(const CachedFileLoader& rhs) = delete;
  // TODO if we need this: come up with a way to make default ctor work
  CachedFileLoader& operator=(CachedFileLoader&& rhs) = delete;
 private:
  /**
   *  Thread function used to load in cache.
   */ 
  void threadfunc_(std::string cache_path);

  /**
   *  Deletes and recreates the cache file.
   */ 
  void recreate_cache_(const std::string& cache_path);

  /**
   *  Sets up the stream used to write to files
   */ 
  void setup_ostream_(const std::string& cache_path);

  loader_progress progress_;                                  // records loading progress
  std::mutex progress_lock_;                                  // must be grabbed when modifying loading progress
  std::unordered_map<std::string, loader_record> cache_;      // internal cache
  std::atomic_bool cached_;                                   // tracks whether map has been cached
  std::condition_variable_any cached_cv_;                     // unlocks when cache is complete
  std::shared_timed_mutex cache_mutex_;                       // mutex associated with cache access
  std::thread load_thread_;                                   // thread used to initially populate cache
  std::fstream cache_file_output_;                            // in-memory cache store
  bool dirty_;                                                // true if we need to update the cache

};

} // namespace file
} // namespace screenspacemanager

#endif  // FILE_LOADER_H_