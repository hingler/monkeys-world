#include <file/CachedFileLoader.hpp>
#include <file/CacheStreambuf.hpp>
#include <utils/FileUtils.hpp>

#include <boost/log/trivial.hpp>

#include <fstream> 
#include <string>

namespace screenspacemanager {
namespace file {

CachedFileLoader::CachedFileLoader(const std::string& cache_name) : cached_(false), dirty_(false) {
  // generate path instead
  std::string cache_path = "resources/cache/" + cache_name;
  load_thread_ = std::thread(&CachedFileLoader::threadfunc_, this, cache_path);
}

loader_progress CachedFileLoader::GetLoaderProgress() {
  std::lock_guard<std::mutex> lock(progress_lock_);
  return {progress_.bytes_read, progress_.bytes_sum};
}

void CachedFileLoader::SpinUntilCached() {
  std::shared_lock<std::shared_timed_mutex> cache_r(cache_mutex_);
  cached_cv_.wait(cache_r, [&]() -> bool{return cached_;});
}

std::unique_ptr<CacheStreambuf> CachedFileLoader::LoadFile(const std::string& path) {
  std::shared_lock<std::shared_timed_mutex> cache_r(cache_mutex_);
  cached_cv_.wait(cache_r, [&]() -> bool{return cached_;});
  std::unordered_map<std::string, loader_record>::const_iterator res_itr = cache_.find(path);

  if (res_itr == cache_.cend()) {
    // release to avoid deadlock up here
    cache_r.release();
    std::unique_lock<std::shared_timed_mutex> cache_g(cache_mutex_);
    res_itr = cache_.find(path);

    if (res_itr == cache_.cend()) {
      dirty_ = true;
      std::ifstream new_cached_file(path, std::ios_base::ate);
      
      loader_record record;
      record.data = std::make_shared<std::vector<char>>();
      record.file_size = new_cached_file.tellg();
      record.data->resize(record.file_size);
      new_cached_file.seekg(std::ios_base::beg, 0);
      new_cached_file.rdbuf()->sgetn(record.data->data(), record.file_size);
      cache_.insert(std::pair<std::string, loader_record>(path, std::move(record)));
      res_itr = cache_.find(path);
    }
  }

  return std::make_unique<CacheStreambuf>(res_itr->second.data);
}

CachedFileLoader::~CachedFileLoader() {
  // write to the cache file
  load_thread_.join();

  if (dirty_) {
    cache_file_output_.seekp(12);
    for (auto cache_itr = cache_.begin(); cache_itr != cache_.end(); cache_itr++) {
      cache_file_output_ << (uint16_t)cache_itr->first.size();
      cache_file_output_.write(cache_itr->first.data(), cache_itr->first.size());
      cache_file_output_ << cache_itr->second.file_size;
    }

    // two passes for now -- maybe we can do it in one :)
    uint32_t cache_crc = utils::fileutils::CalculateCRCHash(cache_file_output_, CACHE_DATA_START);
    cache_file_output_.seekp(CACHE_DATA_START - 8);
    // overwrite the old crc with the new one
    cache_file_output_ << cache_crc;
    cache_file_output_ << static_cast<uint32_t>(cache_.size());
  }

  cache_file_output_.close();
}

CachedFileLoader::CachedFileLoader(CachedFileLoader&& other) {
  std::shared_lock<std::shared_timed_mutex> other_cache_r(other.cache_mutex_);
  other.cached_cv_.wait(other_cache_r, [&]() -> bool{ return other.cached_;});

  cache_ = std::move(other.cache_);
  cache_file_output_ = std::move(other.cache_file_output_);
  std::lock_guard<std::mutex>(other.progress_lock_);
  progress_ = other.progress_;
}

void CachedFileLoader::threadfunc_(std::string cache_path) {
  std::ifstream cache_file(cache_path);
  if (!cache_file.good()) {
    cache_file.close();
    recreate_cache_(cache_path);
    return;
  }

  uint32_t magic;
  cache_file >> magic;
  if (magic != CACHE_MAGIC) {
    cache_file.close();
    recreate_cache_(cache_path);
    return;
  }

  uint32_t crc_expected;
  cache_file >> crc_expected;
  uint32_t crc_actual = utils::fileutils::CalculateCRCHash(cache_file, std::streampos(CACHE_DATA_START));
  if (crc_expected != crc_actual) {
    cache_file.close();
    recreate_cache_(cache_path);
    return;
  }

  uint32_t num_entries;
  cache_file.seekg(CACHE_DATA_START - 4);
  cache_file >> num_entries;
  std::unique_lock<std::shared_timed_mutex>(cache_mutex_);

  // get number of bytes cached
  {
    uint64_t cache_size = 0;
    uint64_t file_size;
    uint16_t pathlen;
    for (int i = 0; i < num_entries; i++) {
      // get total size
      cache_file >> pathlen;
      cache_file.seekg(std::ios_base::cur, pathlen);
      cache_file >> file_size;
      cache_size += file_size;
    }

    // output to progress file
    {
      std::unique_lock<std::mutex>(progress_lock_);
      progress_.bytes_read = 0;
      progress_.bytes_sum = cache_size;
    }
  }

  cache_file.seekg(std::streampos(CACHE_DATA_START));

  // read entries to cache
  while (num_entries--) {
    uint16_t pathlen;
    cache_file >> pathlen;
    std::string path;
    path.resize(pathlen);
    cache_file.read(&path[0], pathlen);

    std::ifstream cached_file(path);
    if (cached_file.bad()) {
      cached_file.close();

      BOOST_LOG_TRIVIAL(error) << "Could not read file at " << path << "; Removing from cache...";
      dirty_ = true;
      continue;
    }

    loader_record cached_file_record;

    cached_file_record.data = std::make_shared<std::vector<char>>();
    cache_file >> cached_file_record.file_size;
    cached_file_record.data->resize(cached_file_record.file_size);

    {
      std::lock_guard<std::mutex> lock(progress_lock_);
      progress_.bytes_read += cached_file_record.file_size;
    }

    cached_file.rdbuf()->sgetn(cached_file_record.data->data(), cached_file_record.file_size);
    cache_.insert(std::pair<std::string, loader_record>(std::move(path), std::move(cached_file_record)));

    cached_file.close();
  
  }
  cache_file.close();

  cached_.store(true, std::memory_order_release);
  cached_cv_.notify_all();
  setup_ostream_(cache_path);
}

void CachedFileLoader::recreate_cache_(const std::string& cache_path) {
  remove(cache_path.c_str());
  // TODO: creating multiple files sucks
  cache_file_output_.open(cache_path, std::ios_base::in |
                                      std::ios_base::out);
  cache_file_output_.seekp(0);
  cache_file_output_ << CACHE_MAGIC;
  cache_file_output_ << 0x00000000;
  cache_file_output_ << static_cast<uint32_t>(0);
}

void CachedFileLoader::setup_ostream_(const std::string& cache_path) {
  cache_file_output_.open(cache_path, std::ios_base::in |
                                      std::ios_base::out);
}

} // namespace file
} // namespace screenspacemanager