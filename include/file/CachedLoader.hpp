#ifndef CACHED_LOADER_H_
#define CACHED_LOADER_H_

#include <string>
#include <vector>

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
  FILE
};

// struct representing a file which may be cached by the loader
struct cache_record {
  CacheType type;
  std::string path;
  uint64_t file_size;
};

/**
 *  Interface for a loader which maintains a cache, and enables files to be loaded into said cache.
 */ 
class CachedLoader {

  /**
   *  @returns a list of all files stored by this cache loader.
   */ 
  virtual std::vector<cache_record> GetCache() = 0;

  /**
   *  @returns how much of the cache has been loaded in thus far
   */ 
  virtual loader_progress GetLoaderProgress() = 0;
  
};

}
}

#endif