#ifndef CACHED_FILE_LOADER_H_
#define CACHED_FILE_LOADER_H_

#include <file/CacheStreambuf.hpp>
#include <file/CachedFileLoader.hpp>
#include <file/CachedLoader.hpp>
#include <file/ModelLoader.hpp>
#include <file/FileLoader.hpp>
#include <file/FontLoader.hpp>

#include <model/Mesh.hpp>
#include <storage/VertexPacketTypes.hpp>

#include <cinttypes>
#include <memory>

namespace monkeysworld {
namespace file {

/**
 *  The CachedFileLoader is designed to streamline the process of loading files,
 *  and displaying loading status.
 * 
 *  On first load: a cache is generated which records all files accessed.
 *  On successive loads: the cache is read, and the files contained in it are loaded automatically.
 *  Cache state may be changed. The cache will always grow but never shrink, just in case.
 * 
 *  To clear the cache: just delete the respective cache file.
 * 
 *  TODO: It looks like there's actually some gains to be made thru multithreading.
 *        It's not a big deal at all but if it comes down to it it might be beneficial lol.
 */ 
class CachedFileLoader {
 public:

  static const uint32_t CACHE_MAGIC = 0x4D534657;   // WFSM
  static const uint64_t CACHE_DATA_START = 12;

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
   *  Loads a file from cache.
   *  @param path - path to the desired file.
   *  @returns - CacheStreambuf associated with the desired file.
   *             If file is invalid, CacheStreambuf::valid will return false.
   */ 
  CacheStreambuf LoadFile(const std::string& path);

  /**
   *  Loads a model from cache.
   *  @param path - path to desired model.
   *  @returns - const ptr to the resulting model.
   *             If model is invalid, returns a nullptr.
   */ 
  std::shared_ptr<const model::Mesh<storage::VertexPacket3D>> LoadModel(const std::string& path);

  std::shared_ptr<const font::Font> LoadFont(const std::string& path);

  ~CachedFileLoader();
  CachedFileLoader(const CachedFileLoader& other) = delete;
  CachedFileLoader(CachedFileLoader&& other) = delete;
  CachedFileLoader& operator=(const CachedFileLoader& rhs) = delete;
  CachedFileLoader& operator=(CachedFileLoader&& rhs) = delete;
 private:

  /**
   *  Generates a vector of cache records.
   */ 
  std::vector<cache_record> ReadCacheFileToVector(const std::string& cache_path);
  
  std::shared_ptr<LoaderThreadPool> thread_pool_;
  std::string cache_path_;
  std::unique_ptr<FileLoader> file_loader_;
  std::unique_ptr<ModelLoader> model_loader_;
  std::unique_ptr<FontLoader> font_loader_;

};

} // namespace file
} // namespace monkeysworld

#endif  // CACHED_FILE_LOADER_H_