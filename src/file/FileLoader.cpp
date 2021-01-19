#include <file/FileLoader.hpp>
#include <boost/log/trivial.hpp>

#include <fstream>

namespace monkeysworld {
namespace file {

FileLoader::FileLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
                       std::vector<cache_record> cache) : CachedLoader(thread_pool) {
  loader_.bytes_read = 0;
  loader_.bytes_sum = 0;

  for (auto record : cache) {
    if (record.type == FILE) {
      loader_.bytes_sum += record.file_size;
      LoadFileToCache(record);
    }
  }
}

std::vector<cache_record> FileLoader::GetCache() {
  std::vector<cache_record> res;
  {
    std::shared_lock<std::shared_timed_mutex> lock(cache_mutex_);
    cache_record temp;
    for (auto record : file_cache_) {
      temp.file_size = record.second->size();
      temp.path = record.first;
      temp.type = CacheType::FILE;
      res.push_back(temp);
    }
  }

  return res;
}

loader_progress FileLoader::GetLoaderProgress() {
  std::unique_lock<std::mutex>(loader_mutex_);
  return loader_;
}

void FileLoader::WaitUntilLoaded() {
  std::unique_lock<std::mutex> lock(loader_mutex_);
  auto lambda = [&] {
    return (loader_.bytes_read == loader_.bytes_sum);
  };

  load_cond_var_.wait(lock, lambda);
}



CacheStreambuf FileLoader::LoadFile(const std::string& path) {
  {
    std::shared_lock<std::shared_timed_mutex> lock(cache_mutex_);
    auto i = file_cache_.find(path);
    if (i != file_cache_.end()) {
      return i->second;
    }
  }
  
  std::ifstream source_stream(path, std::ios_base::in | std::ios_base::binary);
  if (!source_stream.good()) {
    // bad ptr
    BOOST_LOG_TRIVIAL(error) << "bad path for new file";
    return CacheStreambuf();
  }

  auto res = std::make_shared<std::vector<char>>();
  source_stream.seekg(0, std::ios_base::end);
  uint64_t size = source_stream.tellg();
  source_stream.seekg(0, std::ios_base::beg);
  res->resize(size);
  source_stream.rdbuf()->sgetn(res->data(), size);

  {
    std::unique_lock<std::shared_timed_mutex> lock(cache_mutex_);
    file_cache_.insert(std::make_pair(path, res));
  }

  return CacheStreambuf(res);
}

void FileLoader::LoadFileToCache(cache_record& record) {
  auto load_file = [=] {
    std::ifstream source_stream(record.path, std::ios_base::in | std::ios_base::binary);
    if (!source_stream.good()) {
      // skip file, return 0 bytes
      BOOST_LOG_TRIVIAL(error) << "file " << record.path << " could not be cached -- missing";
      // do not cache -- the sync/async function will handle the error :)
      return;
    }

    // TODO: once we've swapped this out for the old loader,
    // use a char ptr and a size instead of a vector.
    // writing directly to data doesn't seem to work as one might hope :(

    // instead of ctor with a vector, ctor with a pointer and a size.
    // then we can avoid calling resize() and having to manually set all of that memory
    auto res = std::make_shared<std::vector<char>>();
    source_stream.seekg(0, std::ios_base::end);
    uint64_t len = source_stream.tellg();
    source_stream.seekg(0, std::ios_base::beg);
    res->resize(len);
    source_stream.rdbuf()->sgetn(res->data(), len);

    {
      std::unique_lock<std::shared_timed_mutex> lock(cache_mutex_);
      file_cache_.insert(std::make_pair(record.path, res));
    }

    {
      std::unique_lock<std::mutex> lock(loader_mutex_);
      // use the file size stored in record, not the new one, if it differs.
      loader_.bytes_read += record.file_size;
      if (loader_.bytes_read >= loader_.bytes_sum) {
        load_cond_var_.notify_all();
      }
    }
  };

  GetThreadPool()->AddTaskToQueue(load_file);
}

}
}