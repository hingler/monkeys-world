#include <file/ModelLoader.hpp>
#include <critter/Model.hpp>

namespace monkeysworld {
namespace file {

ModelLoader::ModelLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
                         std::vector<cache_record> cache) {
  thread_pool_ = thread_pool;
  loader_.bytes_read = 0;
  loader_.bytes_sum = 0;
  for (auto record : cache) {
    // figure out how many bytes we need to read
    loader_.bytes_sum += record.file_size;
  }

  // load the files in
  for (auto record : cache) {
    LoadOBJToCache(record);
  }
}

std::shared_ptr<model::Mesh<storage::VertexPacket3D>> ModelLoader::LoadOBJ(const std::string& path) {
  return LoadOBJFromFile(path);
}

std::future<std::shared_ptr<model::Mesh<>>> ModelLoader::LoadOBJAsync(const std::string& path) {
  
  // neat SO post about packaging up a mutable lambda: https://stackoverflow.com/questions/33436336/capture-stdpromise-in-a-lambda-c14
  // shared ptr to promise gives lambda access
  std::shared_ptr<std::promise<std::shared_ptr<model::Mesh<>>>> result
    = std::make_shared<std::promise<std::shared_ptr<model::Mesh<>>>>();

  auto lambda = [=] {
    // load sync to ptr
    // update promise when received
    auto value = LoadOBJFromFile(path);
    result->set_value(value);
  };

  thread_pool_->AddTaskToQueue(lambda);
  return result->get_future();
}

std::shared_ptr<model::Mesh<>> ModelLoader::LoadOBJFromFile(const std::string& path) {
  {
    std::unique_lock(loader_mutex_);
    if (loader_.bytes_read != loader_.bytes_sum) {
      // more content to load
      // TODO: Write an exception to throw here instead of returning nullptr
      return std::shared_ptr<model::Mesh<>>();
    }
  }

  {
    // wait until the cache is done loading :)
    std::shared_lock<std::shared_timed_mutex>(cache_mutex_);
    auto i = model_cache_.find(path);
    if (i != model_cache_.end()) {
      return i->second.ptr;
    }
  }

  // need to get file size from path
  // let's write, like, a really stupid solution for now lol
  // TODO: move `FromObjFile` here so that we can fuck with it more.
  //       and instead of returning a shared ptr to a mesh, return our model_record instead.
  
  // sound of alarm going off
  std::ifstream file(path);
  file.seekg(std::ios_base::end, 0);
  uint64_t size = file.tellg();
  file.close();

  std::shared_ptr<model::Mesh<>> mesh = critter::Model::FromObjFile(path);
  model_record record = {mesh, size};

  {
    std::unique_lock(cache_mutex_);
    model_cache_.insert(std::make_pair(path, record));
  }
}

std::vector<cache_record> ModelLoader::GetCache() {
  std::vector<cache_record> result;
  // store something which preserves file size
  {
    std::shared_lock(cache_mutex_);
    cache_record record_temp;
    for (auto entry : model_cache_) {
      record_temp.file_size = entry.second.size;
      record_temp.path = entry.first;
      record_temp.type = CacheType::MODEL;
      result.push_back(record_temp);
    }
  }

  return result;
}

loader_progress ModelLoader::GetLoaderProgress() {
  std::lock_guard(loader_mutex_);
  return loader_;
}

void ModelLoader::LoadOBJToCache(cache_record& record) {
  auto load_model = [=] {
    auto result = critter::Model::FromObjFile(record.path);
    model_record cache = {result, record.file_size};
    {
      std::unique_lock(cache_mutex_);
      model_cache_.insert(std::make_pair(record.path, cache));
    }

    {
      std::unique_lock(this->loader_mutex_);
      loader_.bytes_read += record.file_size;
    }

  };

  thread_pool_->AddTaskToQueue(load_model);
}

}
}