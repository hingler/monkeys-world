#include <file/FontLoader.hpp>
#include <font/exception/BadFontPathException.hpp>

namespace monkeysworld {
namespace file {

FontLoader::FontLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
                       std::vector<cache_record> cache) : CachedLoader(thread_pool) {
  loader_.bytes_read = 0;
  loader_.bytes_sum = 0;
  for (auto record : cache) {
    if (record.type == FONT) {
      loader_.bytes_sum++;
      LoadFontToCache(record);
    }
  }
}

std::shared_ptr<font::Font> FontLoader::LoadFile(const std::string& path) {
  return LoadFromFile(path);
}

std::future<std::shared_ptr<font::Font>> FontLoader::LoadFileAsync(const std::string& path) {
  std::shared_ptr<std::promise<std::shared_ptr<font::Font>>> result
    = std::make_shared<std::promise<std::shared_ptr<font::Font>>>();
  auto load_async_lambda = [=] {
    auto ptr = LoadFromFile(path);
    result->set_value(ptr);
  };

  GetThreadPool()->AddTaskToQueue(load_async_lambda);
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
  if (loader_.bytes_read < loader_.bytes_sum) {
    auto lambda = [&]() -> bool {
      return (loader_.bytes_read == loader_.bytes_sum);
    };

    load_cond_var_.wait(lock, lambda);
  }
}

std::shared_ptr<font::Font> FontLoader::LoadFromFile(const std::string& path) {
  std::shared_ptr<font::Font> res;
  {
    std::shared_lock<std::shared_timed_mutex> lock(cache_mutex_);
    auto i = font_cache_.find(path);
    if (i != font_cache_.end()) {
      return i->second;
    }
  }

  // not catching this exception -- im gonna let it bump up and be public
  // TBA: in the event of an exception from this call, return a shitty default font
  res = std::make_shared<font::Font>(path);

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

  GetThreadPool()->AddTaskToQueue(std::move(load_font));
}

}
}