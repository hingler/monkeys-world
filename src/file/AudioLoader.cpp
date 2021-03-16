#include <file/AudioLoader.hpp>
#include <boost/algorithm/string.hpp>

#include <audio/AudioBufferOgg.hpp>

#define SAMPLE_COUNT 16384

namespace monkeysworld {
namespace file {

AudioLoader::AudioLoader(std::shared_ptr<LoaderThreadPool> thread_pool, std::vector<cache_record> cache) : CachedLoader(thread_pool) {
  loader_.bytes_read = loader_.bytes_sum = 0;
  for (auto record : cache) {
    if (record.type == AUDIO) {
      LoadFileToCache(record);
    }
  }
}

std::vector<cache_record> AudioLoader::GetCache() {
  std::vector<cache_record> res;
  cache_record temp;
  std::unique_lock<std::mutex> lock(cache_mutex_);
  for (const auto& record : cache_) {
    temp.file_size = record.second.sample_count;
    temp.path = record.first;
    temp.type = AUDIO;
    res.push_back(temp);
  }

  lock.unlock();

  return res;
}

loader_progress AudioLoader::GetLoaderProgress() {
  std::unique_lock<std::mutex> lock(loader_mutex_);
  return loader_;
}

void AudioLoader::WaitUntilLoaded() {
  std::unique_lock<std::mutex> lock(loader_mutex_);
  if (loader_.bytes_read < loader_.bytes_sum) {
    auto lambda = [&]() -> bool {
      return (loader_.bytes_read >= loader_.bytes_sum);
    };

    load_cond_var_.wait(lock, lambda);
  }
}

std::shared_ptr<audio::AudioBuffer> AudioLoader::LoadFile(const std::string& path) {
  return LoadFromPath(path);
}

bool AudioLoader::IsCached(const std::string& path) {
  std::unique_lock<std::mutex> lock(cache_mutex_);
  return (cache_.find(path) != cache_.end());
}

std::shared_ptr<audio::AudioBuffer> AudioLoader::LoadFromPath(const std::string& path) {
  std::vector<std::string> suffix;
  boost::split(suffix, path, [](char c){ return (c == '.'); }, boost::token_compress_on);
  if (suffix.size() == 0) {
    BOOST_LOG_TRIVIAL(error) << "file type unknown for " << path;
    return nullptr;
  }

  std::string file_type = suffix[suffix.size() - 1];
  boost::to_lower(file_type);
  BOOST_LOG_TRIVIAL(trace) << "loading audio file: ." << file_type;
  std::shared_ptr<audio::AudioBuffer> res;

  if (file_type == "ogg") {
    res = std::make_shared<audio::AudioBufferOgg>(SAMPLE_COUNT, path);
  } else {
    BOOST_LOG_TRIVIAL(error) << "invalid file type " << file_type;
    return nullptr;
  }

  if (IsCached(path)) {
    std::unique_lock<std::mutex> lock(cache_mutex_);
    auto entry = cache_.find(path)->second;
    BOOST_LOG_TRIVIAL(trace) << "loading " << entry.sample_count << " samples from cache";
    res->Write(entry.sample_count, entry.left, entry.right);
  } else {
    AudioCache cache;
    int bytes_written = res->WriteFromFile(SAMPLE_COUNT);
    cache.sample_count = bytes_written;
    cache.left = new float[bytes_written];
    cache.right = new float[bytes_written];
    res->Peek(bytes_written, cache.left, cache.right);
    std::unique_lock<std::mutex> lock(cache_mutex_);
    cache_.insert(std::make_pair(path, cache));
  }

  return res;
}


void AudioLoader::LoadFileToCache(cache_record& record) {
  // this handles caching -- just ignore the result it produces.
  auto lambda = [&, record] {
    LoadFromPath(record.path);
    std::unique_lock<std::mutex> lock(loader_mutex_);
    loader_.bytes_read += record.file_size;
    if (loader_.bytes_read >= loader_.bytes_sum) {
      load_cond_var_.notify_all();
    }
  };

  GetThreadPool()->AddTaskToQueue(lambda);
}

AudioLoader::~AudioLoader() {
  std::unique_lock<std::mutex> lock(cache_mutex_);
  for (auto entry : cache_) {
    delete entry.second.left;
    delete entry.second.right;
  }
}

}
}