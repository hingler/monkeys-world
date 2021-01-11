#include <file/TextureLoader.hpp>
#include <shader/exception/InvalidTexturePathException.hpp>

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace file {

TextureLoader::TextureLoader(std::shared_ptr<LoaderThreadPool> thread_pool, std::vector<cache_record> cache) {
  thread_pool_ = thread_pool;
  loader_.bytes_read = 0;
  loader_.bytes_sum = 0;
  for (auto record : cache) {
    if (record.type == TEXTURE) {
      loader_.bytes_sum += record.file_size;
      LoadTextureToCache(record);
    }
  }
}

std::shared_ptr<shader::Texture> TextureLoader::LoadTexture(const std::string& path) {
  return LoadTextureFromFile(path);
}

std::future<std::shared_ptr<shader::Texture>> TextureLoader::LoadTextureAsync(const std::string& path) {
  std::shared_ptr<std::promise<std::shared_ptr<shader::Texture>>> p
    = std::make_shared<std::promise<std::shared_ptr<shader::Texture>>>();
  auto lambda = [=] {
    auto t = LoadTextureFromFile(path);
    p->set_value(t);
  };

  thread_pool_->AddTaskToQueue(lambda);
  return std::move(p->get_future());
}

std::vector<cache_record> TextureLoader::GetCache() {
  std::vector<cache_record> res;
  {
    std::shared_lock<std::shared_timed_mutex>(cache_mutex_);
    cache_record temp;
    for (auto entry : texture_cache_) {
      temp.file_size = entry.second->GetTextureSize();
      temp.path = entry.first;
      temp.type = TEXTURE;
      res.push_back(temp);
    }
  }

  return res;
}

loader_progress TextureLoader::GetLoaderProgress() {
  std::unique_lock<std::mutex>(loader_mutex_);
  return loader_;
}

void TextureLoader::WaitUntilLoaded() {
  std::unique_lock<std::mutex> lock(loader_mutex_);
  if (loader_.bytes_read != loader_.bytes_sum) {
    auto lambda = [&]() -> bool {
      auto prog = GetLoaderProgress();
      return (prog.bytes_read == prog.bytes_sum);
    };

    load_cond_var_.wait(lock, lambda);
  }
}

std::shared_ptr<shader::Texture> TextureLoader::LoadTextureFromFile(const std::string& path) {

  {
    std::shared_lock<std::shared_timed_mutex>(cache_mutex_);
    auto i = texture_cache_.find(path);
    if (i != texture_cache_.end()) {
      return i->second;
    }
  }

  std::shared_ptr<shader::Texture> t;
  try {
    t = std::make_shared<shader::Texture>(path);
  } catch (shader::exception::InvalidTexturePathException e) {
    BOOST_LOG_TRIVIAL(warning) << "Texture " << path << " unable to be loaded.";
    return std::shared_ptr<shader::Texture>(nullptr);
  }

  {
    std::unique_lock<std::shared_timed_mutex>(cache_mutex_);
    texture_cache_.insert(std::make_pair(path, t));
  }

  return t;
}

void TextureLoader::LoadTextureToCache(const cache_record& record) {
  auto lambda = [=] {
    std::shared_ptr<shader::Texture> t;
    try {
      t = std::make_shared<shader::Texture>(record.path);
    } catch (shader::exception::InvalidTexturePathException e) {
      // invalid path
      BOOST_LOG_TRIVIAL(warning) << "While caching: File " << record.path << " not found";
      return;
    }

    {
      std::unique_lock<std::shared_timed_mutex>(cache_mutex_);
      texture_cache_.insert(std::make_pair(record.path, t));
    }

    {
      std::unique_lock<std::mutex>(loader_mutex_);
      loader_.bytes_read += t->GetTextureSize();

      if (loader_.bytes_read >= loader_.bytes_sum) {
        load_cond_var_.notify_all();
      }
    }
  };

  thread_pool_->AddTaskToQueue(lambda);
}

}
}