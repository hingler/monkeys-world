#include <file/CubeMapLoader.hpp>

#include <boost/algorithm/string.hpp>

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace file {

CubeMapLoader::CubeMapLoader(std::shared_ptr<LoaderThreadPool> thread_pool, std::vector<cache_record> cache) 
  : CachedLoader(thread_pool) {
  loader_.bytes_read = 0;
  loader_.bytes_sum = 0;
  for (auto record : cache) {
    if (record.type == CUBEMAP) {
      loader_.bytes_sum += record.file_size;
      LoadFileToCache(record);
    }
  }
}

std::vector<cache_record> CubeMapLoader::GetCache() {
  std::shared_lock<std::shared_timed_mutex> lock(cache_mutex_);
  cache_record temp;
  std::vector<cache_record> res;
  for (auto i : file_cache_) {
    temp.file_size = i.second->GetCubeMapSize();
    temp.path = i.first;
    temp.type = CUBEMAP;
    res.push_back(temp);
  }

  return res;
}

loader_progress CubeMapLoader::GetLoaderProgress() {
  std::unique_lock<std::mutex> lock(loader_mutex_);
  return loader_;
}

void CubeMapLoader::WaitUntilLoaded() {
  std::unique_lock<std::mutex> lock(loader_mutex_);
  if (loader_.bytes_read < loader_.bytes_sum) {
    auto lambda = [&]() -> bool {
      return (loader_.bytes_read == loader_.bytes_sum);
    };
    load_cond_var_.wait(lock, lambda);
  }
}

std::shared_ptr<shader::CubeMap> CubeMapLoader::LoadFromFile(const std::string& path) {
  // split file apart on colons (check for first char is colon)
  // pass each one to the thing
  {
    std::shared_lock<std::shared_timed_mutex> lock(cache_mutex_);
    auto i = file_cache_.find(path);
    if (i != file_cache_.end()) {
      return i->second;
    }
  }

  if (path[0] == ':') {
    // note: this behavior is intentional
    std::vector<std::string> paths;
    boost::split(paths, path, [](char c){ return (c == ':'); }, boost::token_compress_on);
    // first colon, identifying split string, forms an empty group.
    // ignore this :)
    if (paths.size() != 7) {
      BOOST_LOG_TRIVIAL(error) << "string does not contain correct number of paths -- expected 7, actual " << paths.size();
      BOOST_LOG_TRIVIAL(error) << path;
      for (auto s : paths) {
        BOOST_LOG_TRIVIAL(error) << s;
      }
    }

    auto cubemap = std::make_shared<shader::CubeMap>(paths[1], paths[2], paths[3], paths[4], paths[5], paths[6]);

    {
      std::unique_lock<std::shared_timed_mutex> lock(cache_mutex_);
      file_cache_.insert(std::make_pair(path, cubemap));
    }

    return cubemap;
  } else {
    BOOST_LOG_TRIVIAL(warning) << "expected leading ':' in passed filename";
    // haven't implemented cubemaps for single-file because i dont want to yet
    return std::shared_ptr<shader::CubeMap>(nullptr);
  }
}

void CubeMapLoader::LoadFileToCache(cache_record& record) {
  if (record.path[0] == ':') {
    std::vector<std::string> paths;
    boost::split(paths, record.path, [](char c){ return (c == ':'); }, boost::token_compress_on);
    // ensure path length is equal to 6
    if (paths.size() != 6) {
      BOOST_LOG_TRIVIAL(error) << "string does not contain correct number of paths -- expected 6, actual " << paths.size();
      BOOST_LOG_TRIVIAL(error) << record.path;
    }

    auto cubemap = std::make_shared<shader::CubeMap>(paths[0], paths[1], paths[2], paths[3], paths[4], paths[5]);

    {
      std::unique_lock<std::shared_timed_mutex> lock(cache_mutex_);
      file_cache_.insert(std::make_pair(record.path, cubemap));
    }
  } else {
    BOOST_LOG_TRIVIAL(warning) << "expected leading ':' in passed filename";
  }

  {
    std::unique_lock<std::mutex> lock(loader_mutex_);
    loader_.bytes_read += record.file_size;
    if (loader_.bytes_read >= loader_.bytes_sum) {
      load_cond_var_.notify_all();
    }
  }
}

}
}