#ifndef MODEL_LOADER_H_
#define MODEL_LOADER_H_

#include <model/Mesh.hpp>
#include <storage/VertexPacketTypes.hpp>
#include <file/LoaderThreadPool.hpp>
#include <file/CachedLoader.hpp>

#include <future>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace monkeysworld {
namespace file {

/**
 *  Utility for loading simple 3Dmodels from file.
 *  Stores models in an internal cache, which associates models with file paths.
 *  TBA: rigged models will be more complex -- all I can do right now is OBJs.
 *  At some point soon, figure out how to use more complex formats instead :)
 */ 
class ModelLoader : public CachedLoader {
 public:
  /**
   *  Creates a new model loader
   *  @param thread_pool - thread pool shared across loaders.
   *  @param cache - a list of cached files previously associated with this loader.
   */ 
  ModelLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
              std::vector<cache_record> cache);

  /**
   *  Synchronously loads an OBJ file.
   *  @param path - path to the new obj file.
   *  @returns resulting 3D model.
   */ 
  std::shared_ptr<model::Mesh<storage::VertexPacket3D>> LoadOBJ(const std::string& path);

  /**
   *  Asynchronously loads an OBJ file via a returned promise.
   *  @param path - path to the new obj file.
   *  @returns promise which will eventually resolve to the desired 3D model.
   */ 
  std::future<std::shared_ptr<model::Mesh<storage::VertexPacket3D>>> LoadOBJAsync(const std::string& path);

  /**
   *  @returns a list of cache_records associated with this loader.
   */ 
  std::vector<cache_record> GetCache() override;

  /**
   *  @returns how much of our loader cache has been populated thus far.
   */ 
  loader_progress GetLoaderProgress() override;

 private:
  struct model_record {
    std::shared_ptr<model::Mesh<>> ptr;
    uint64_t size;
  };

  /**
   *  Underlying function which loads obj either synchronously or asynchronously.
   *  Also handles placing the OBJ file in the cache.
   */ 
  std::shared_ptr<model::Mesh<storage::VertexPacket3D>> LoadOBJFromFile(const std::string& path);

  /**
   *  Similar to above, but populates the cache directly instead of returning a promise.
   */ 
  void LoadOBJToCache(cache_record& record);

  loader_progress loader_;
  std::mutex loader_mutex_;
  std::shared_timed_mutex cache_mutex_;
  std::unordered_map<std::string, model_record> model_cache_;
  std::shared_ptr<LoaderThreadPool> thread_pool_;



};

}
}

#endif