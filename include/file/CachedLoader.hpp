#ifndef CACHED_LOADER_H_
#define CACHED_LOADER_H_

#include <file/LoaderThreadPool.hpp>

#include <condition_variable>
#include <future>
#include <mutex>
#include <string>
#include <vector>

// Re-TBA: Use CRTP to call a loader function belonging to the child.
// this'll clobber our loading method but it's a nop anyway since we can't do anything!
namespace monkeysworld {
namespace file {

struct loader_progress {
  size_t bytes_read;          // number of bytes read thus far
  size_t bytes_sum;           // number of bytes expected
};

enum CacheType {
  MODEL = 0,
  FONT,
  TEXTURE,
  FILE,
  CUBEMAP
};

// struct representing a file which may be cached by the loader
struct cache_record {
  CacheType type;
  std::string path;
  uint64_t file_size;
};

/**
 *  Interface for a loader which maintains a cache, and enables files to be loaded into said cache.
 *  @param T - The type of data being returned by the loader.
 *  @param U - A reference to the derived class.
 */ 
template <typename T, typename U>
class CachedLoader {
 public:

  /**
   *  Constructs the cached loader.
   *  @param thread_pool - pool of threads for loading asynchronously.
   */ 
  CachedLoader(std::shared_ptr<LoaderThreadPool> thread_pool) {
    thread_pool_ = thread_pool;
  }

  /**
   *  Synchronously loads the desired file.
   *  @param path - path to the desired file.
   *  @returns Instance of type T associated with the desired file.
   */ 
  T LoadFile(const std::string& path) {
    return LoadFromFile(path);
  }

  /**
   *  Asynchronously loads the desired file.
   *  @param path - path to the desired file.
   *  @returns future which will resolve to instance of type T when load is complete.
   */ 
  std::future<T> LoadFileAsync(const std::string& path) {
    std::shared_ptr<std::promise<T>> p = std::make_shared<std::promise<T>>();
    auto lambda = [=] {
      T res = LoadFromFile(path);
      p->set_value(res);
    };

    thread_pool_->AddTaskToQueue(lambda);
    return std::move(p->get_future());
  }

  /**
   *  @returns a list of all files stored by this cache loader.
   */ 
  virtual std::vector<cache_record> GetCache() = 0;

  /**
   *  On creation, before cache is loaded,
   *  loaders should ensure that they return *something* resembling 0%.
   *  @returns how much of the cache has been loaded in thus far
   */ 
  virtual loader_progress GetLoaderProgress() = 0;

  /**
   *  Returns once cache loading is complete.
   */ 
  virtual void WaitUntilLoaded() = 0;
 protected:
  T LoadFromFile(const std::string& path) {
    U* derived_ptr = static_cast<U*>(this);
    return derived_ptr->LoadFromFile(path);
  }

  std::shared_ptr<LoaderThreadPool>& GetThreadPool() {
    return thread_pool_;
  }

 private:
  std::shared_ptr<LoaderThreadPool> thread_pool_;
  
  
};

}
}

#endif