#include <file/FontLoader.hpp>
#include <font/exception/BadFontPathException.hpp>

namespace monkeysworld {
namespace file {

FontLoader::FontLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
                       std::vector<cache_record> cache) {
  thread_pool_ = thread_pool;
  loader_.bytes_read = 0;
  loader_.bytes_sum = 0;
  for (auto record : cache) {
    if (record.type == FONT) {
      loader_.bytes_sum++;
      LoadFontToCache(record);
    }
  }
}

std::shared_ptr<font::Font> FontLoader::LoadFont(const std::string& path) {
  return LoadFontFromFile(path);
}

std::future<std::shared_ptr<font::Font>> FontLoader::LoadFontAsync(const std::string& path) {
  std::shared_ptr<std::promise<std::shared_ptr<font::Font>>> result
    = std::make_shared<std::promise<std::shared_ptr<font::Font>>>();
  auto load_async_lambda = [=] {
    auto ptr = LoadFontFromFile(path);
    result->set_value(ptr);
  };

  thread_pool_->AddTaskToQueue(load_async_lambda);
  return result->get_future();
}

std::vector<cache_record> FontLoader::GetCache() {
  std::shared_lock<std::shared_timed_mutex> lock(cache_mutex_);
  std::vector<cache_record> result;
  cache_record temp;
  for (auto record : font_cache_) {
    temp.path = record.first;
    temp.file_size = 1;       // placeholder, doesn't really matter
    temp.type = CacheType::FONT;
    result.push_back(temp);
  }

  return result;
}

loader_progress FontLoader::GetLoaderProgress() {
  std::unique_lock<std::mutex> lock(loader_mutex_);
  return loader_;
}

void FontLoader::WaitUntilLoaded() {
  std::unique_lock<std::mutex> lock(loader_mutex_);
  if (loader_.bytes_read != loader_.bytes_sum) {
    auto lambda = [&]() -> bool {
      auto prog = GetLoaderProgress();
      return (prog.bytes_read == prog.bytes_sum);
    };

    load_cond_var_.wait(lock, lambda);
  }
}

std::shared_ptr<font::Font> FontLoader::LoadFontFromFile(const std::string& path) {
  std::shared_ptr<font::Font> res;
  {
    std::shared_lock<std::shared_timed_mutex> lock(cache_mutex_);
    auto i = font_cache_.find(path);
    if (i != font_cache_.end()) {
      return i->second;
    }
  }

  try {
    res = std::make_shared<font::Font>(path);
  } catch (font::exception::BadFontPathException e) {
    return std::shared_ptr<font::Font>(nullptr);
  }

  {
    std::unique_lock<std::shared_timed_mutex> lock(cache_mutex_);
    font_cache_.insert(std::make_pair(path, res));
  }

  return res;
}

void FontLoader::LoadFontToCache(cache_record& record) {
  auto load_font = [=] {
    std::shared_ptr<font::Font> res;

    try {
      res = std::make_shared<font::Font>(record.path);
    } catch (font::exception::BadFontPathException e) {
      BOOST_LOG_TRIVIAL(trace) << "Could not load font " << record.path;
      return;
    }

    {
      std::unique_lock<std::shared_timed_mutex> lock(cache_mutex_);
      font_cache_.insert(std::make_pair(record.path, res));
    }

    {
      std::unique_lock<std::mutex> lock(loader_mutex_);
      loader_.bytes_read++;
      if (loader_.bytes_read >= loader_.bytes_sum) {
        load_cond_var_.notify_all();
      }
    }
  };

  thread_pool_->AddTaskToQueue(std::move(load_font));
}

}
}