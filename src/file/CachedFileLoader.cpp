#include <file/CachedFileLoader.hpp>
#include <file/CacheStreambuf.hpp>
#include <utils/FileUtils.hpp>

#include <boost/log/trivial.hpp>

#include <fstream> 
#include <string>

namespace monkeysworld {
namespace file {

using utils::fileutils::WriteAsBytes;
using utils::fileutils::ReadAsBytes;

CachedFileLoader::CachedFileLoader(const std::string& cache_name) : cached_(false), dirty_(false) {
  // generate path instead
  std::string cache_path = "resources/cache/" + cache_name + ".filecache";
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

std::unique_ptr<std::streambuf> CachedFileLoader::LoadFile(const std::string& path) {
  std::shared_lock<std::shared_timed_mutex> cache_r(cache_mutex_);
  while (!cached_) {
    cached_cv_.wait(cache_r, [&]() -> bool{return cached_;});
  }

  std::unordered_map<std::string, loader_record>::const_iterator res_itr = cache_.find(path);

  if (res_itr == cache_.cend()) {
    // unlock to avoid deadlock up here
    // release dissociates the mutex, unlock actually does what i want
    cache_r.unlock();

    std::unique_lock<std::shared_timed_mutex> cache_g(cache_mutex_);
    res_itr = cache_.find(path);

    if (res_itr == cache_.cend()) {
      std::ifstream new_cached_file(path, std::ios_base::ate | std::ios_base::binary);
      // no handle for invalid file
      if (new_cached_file.fail()) {
        // file does not exist!
        BOOST_LOG_TRIVIAL(error) << "Encountered invalid file " << path;
        return std::unique_ptr<std::streambuf>(nullptr);
      }
      dirty_ = true;
      new_cached_file.seekg(0, new_cached_file.end);
      
      loader_record record;
      record.data = std::make_shared<std::vector<char>>();
      record.file_size = new_cached_file.tellg();
      BOOST_LOG_TRIVIAL(trace) << "Loading file " << path << " - " << record.file_size << " bytes";
      record.data->resize(record.file_size);
      new_cached_file.seekg(new_cached_file.beg, 0);
      new_cached_file.rdbuf()->sgetn(record.data->data(), record.file_size);
      cache_.insert(std::make_pair(path, std::move(record)));
      res_itr = cache_.find(path);
    }
  }

  // unique ptr assumes ownership of the newly allocated streambuf
  CacheStreambuf* result_streambuf = new CacheStreambuf(res_itr->second.data);
  return std::unique_ptr<std::streambuf>(result_streambuf);
}

CachedFileLoader::~CachedFileLoader() {
  // write to the cache file
  load_thread_.join();

  if (dirty_) {
    BOOST_LOG_TRIVIAL(debug) << "Updating cache...";
    cache_file_output_.seekp(12);
    for (auto cache_itr = cache_.begin(); cache_itr != cache_.end(); cache_itr++) {
      BOOST_LOG_TRIVIAL(trace) << "Removing " << cache_itr->first.data();
      uint16_t path_size = static_cast<uint16_t>(cache_itr->first.size());
      WriteAsBytes(cache_file_output_, path_size);
      cache_file_output_.write(cache_itr->first.data(), cache_itr->first.size());
      WriteAsBytes(cache_file_output_, cache_itr->second.file_size);
    }

    // two passes for now -- maybe we can do it in one :)
    uint32_t cache_crc = utils::fileutils::CalculateCRCHash(cache_file_output_, CACHE_DATA_START);
    cache_file_output_.seekp(CACHE_DATA_START - 8);
    // overwrite the old crc with the new one
    
    WriteAsBytes(cache_file_output_, cache_crc);
    WriteAsBytes(cache_file_output_, static_cast<uint32_t>(cache_.size()));
  }

  BOOST_LOG_TRIVIAL(debug) << "Closing cache file...";
  cache_file_output_.close();
}

CachedFileLoader::CachedFileLoader(CachedFileLoader&& other) {
  std::shared_lock<std::shared_timed_mutex> other_cache_r(other.cache_mutex_);
  while (!other.cached_) {
    other.cached_cv_.wait(other_cache_r, [&]() -> bool{ return other.cached_;});
  }

  cache_ = std::move(other.cache_);
  cache_file_output_ = std::move(other.cache_file_output_);
  std::lock_guard<std::mutex>(other.progress_lock_);
  progress_ = other.progress_;
}

void CachedFileLoader::threadfunc_(std::string cache_path) {
  std::ifstream cache_file(cache_path, std::ios_base::in | std::ios_base::binary);
  if (!cache_file.good()) {
    cache_file.close();
    BOOST_LOG_TRIVIAL(warning) << "Cache file not found. Recreating cache...";
    recreate_cache_(cache_path);
    setup_ostream_(cache_path);
    return;
  }

  uint32_t magic = ReadAsBytes<uint32_t>(cache_file);
  if (magic != CACHE_MAGIC) {
    BOOST_LOG_TRIVIAL(warning) << "Magic number did not match. Recreating cache...";
    cache_file.close();
    recreate_cache_(cache_path);
    setup_ostream_(cache_path);
    return;
  }

  uint32_t crc_expected = ReadAsBytes<uint32_t>(cache_file);
  uint32_t crc_actual = utils::fileutils::CalculateCRCHash(cache_file, std::streampos(CACHE_DATA_START));
  if (crc_expected != crc_actual) {
    BOOST_LOG_TRIVIAL(warning) << "Old CRC did not match. Recreating cache...";
    cache_file.close();
    recreate_cache_(cache_path);
    setup_ostream_(cache_path);
    return;
  }

  uint32_t num_entries = ReadAsBytes<uint32_t>(cache_file);
  std::unique_lock<std::shared_timed_mutex>(cache_mutex_);
  BOOST_LOG_TRIVIAL(debug) << "Cache file with " << num_entries << " entries found";

  // get number of bytes cached
  {
    uint64_t cache_size = 0;
    uint64_t file_size;
    uint16_t pathlen;
    for (int i = 0; i < num_entries; i++) {
      // get total size

      pathlen = ReadAsBytes<uint16_t>(cache_file);
      cache_file.seekg(pathlen, std::ios_base::cur);
      file_size = ReadAsBytes<uint64_t>(cache_file);
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
    uint16_t pathlen = ReadAsBytes<uint16_t>(cache_file);
    std::string path;
    path.resize(pathlen);
    cache_file.read(&path[0], pathlen);

    std::ifstream cached_file(path, std::ios_base::in | std::ios_base::binary);
    if (cached_file.bad()) {
      cached_file.close();

      BOOST_LOG_TRIVIAL(error) << "Could not read file at " << path << "; Removing from cache...";
      dirty_ = true;
      continue;
    }

    loader_record cached_file_record;

    cached_file_record.data = std::make_shared<std::vector<char>>();
    cached_file_record.file_size = ReadAsBytes<size_t>(cache_file);
    cached_file_record.data->resize(cached_file_record.file_size);

    BOOST_LOG_TRIVIAL(trace) << "Read file at " << path << " - " << cached_file_record.file_size << " bytes";

    {
      std::lock_guard<std::mutex> lock(progress_lock_);
      progress_.bytes_read += cached_file_record.file_size;
    }

    cached_file.rdbuf()->sgetn(cached_file_record.data->data(), cached_file_record.file_size);
    cache_.insert(std::make_pair(std::move(path), std::move(cached_file_record)));

    cached_file.close();
  
  }
  cache_file.close();
  setup_ostream_(cache_path);
}

void CachedFileLoader::recreate_cache_(const std::string& cache_path) {
  remove(cache_path.c_str());
  // input mode requires file to exist.
  std::ofstream new_cache_file(cache_path, std::ios_base::out | std::ios_base::binary);
  new_cache_file.seekp(0);
  WriteAsBytes(new_cache_file, CACHE_MAGIC);
  // crc and num_entries should be 0.
  uint64_t filler = 0;
  WriteAsBytes(new_cache_file, filler);
  BOOST_LOG_TRIVIAL(debug) << "Cache created at " << cache_path;
  new_cache_file.close();
}

void CachedFileLoader::setup_ostream_(const std::string& cache_path) {
  cache_file_output_.open(cache_path, std::fstream::in |
                                      std::fstream::out |
                                      std::fstream::binary);
  cached_.store(true, std::memory_order_seq_cst);
  // reordered calls could leave a thread waiting forever
  // the bool store should prevent that but whatever lol better safe than not
  std::unique_lock<std::shared_timed_mutex> cache_notify(cache_mutex_);
  cached_cv_.notify_all();
}

} // namespace file
} // namespace monkeysworld