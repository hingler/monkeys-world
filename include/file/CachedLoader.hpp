#ifndef CACHED_LOADER_H_
#define CACHED_LOADER_H_

#include <file/LoaderThreadPool.hpp>

#include <boost/log/trivial.hpp>

#include <condition_variable>
#include <future>
#include <mutex>
#include <string>
#include <unordered_map>
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

  /**
   *  Returned by implementors.
   *  Returns true if the provided path is cached.
   *  False otherwise.
   */ 
  virtual bool IsCached(const std::string& path) = 0;
 protected:
  // implement synchronous loading in LoadFile
  T LoadFromFile(const std::string& path) {
    // ensure cache is complete
    WaitUntilLoaded();

    bool load_fresh = false;
    U* derived_ptr = static_cast<U*>(this);
    // check cache here
    {
      std::unique_lock<std::mutex> lock(loads_lock_);
      auto rec = loads_.find(path);
      if (rec != loads_.end()) {
        // someone is loading for the first time, RIGHT NOW
        auto cond_var = rec->second;
        cond_var->wait(lock, [&] { return IsCached(path); });
      } else if (!IsCached(path)) {
        // no one's ever loaded this before -- create a record to let everyone know
        // that we're loading for the first time
        load_fresh = true;
        loads_.insert(std::make_pair(path, std::make_shared<std::condition_variable>()));
      }
    }

    BOOST_LOG_TRIVIAL(trace) << "loading your model: " << path;
    auto res = derived_ptr->LoadFile(path);

    if (load_fresh) {
      BOOST_LOG_TRIVIAL(trace) << "removing from load list...";
      // first load, so we created a record. remove that record now :)
      std::unique_lock<std::mutex> lock(loads_lock_);
      auto rec = loads_.find(path);
      auto cond_var = rec->second;
      loads_.erase(rec);
      cond_var->notify_all();
    }

    return res;
  }

  std::shared_ptr<LoaderThreadPool>& GetThreadPool() {
    return thread_pool_;
  }

 private:
  std::shared_ptr<LoaderThreadPool> thread_pool_;

  std::unordered_map<std::string, std::shared_ptr<std::condition_variable>> loads_;
  std::mutex loads_lock_;
  
  
};

}
}

#endif