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

CachedFileLoader::CachedFileLoader(const std::string& cache_name) {
  cache_path_ = "resources/cache/" + cache_name + ".cache";
  auto cache = ReadCacheFileToVector(cache_path_);
  thread_pool_ = std::make_shared<LoaderThreadPool>(8);
  file_loader_ = std::make_unique<FileLoader>(thread_pool_, cache);
}

loader_progress CachedFileLoader::GetLoaderProgress() {
  return file_loader_->GetLoaderProgress();
}

CacheStreambuf CachedFileLoader::LoadFile(const std::string& path) {
  return file_loader_->LoadFile(path);
}

std::vector<cache_record> CachedFileLoader::ReadCacheFileToVector(const std::string& cache_path) {
  std::vector<cache_record> record;
  std::ifstream cache(cache_path);
  if (!cache.good()) {
    BOOST_LOG_TRIVIAL(warning) << "cache is invalid!";
    // return an empty vector for now
    cache.close();
    return record;
  }

  uint32_t magic = ReadAsBytes<uint32_t>(cache);
  if (magic != CACHE_MAGIC) {
    BOOST_LOG_TRIVIAL(warning) << "cache validation failed: bad magic";
    cache.close();
    return record;
  }

  uint32_t crc_expected = ReadAsBytes<uint32_t>(cache);
  uint32_t crc_actual = utils::fileutils::CalculateCRCHash(cache, std::streampos(CACHE_DATA_START));
  if (crc_expected != crc_actual) {
    BOOST_LOG_TRIVIAL(warning) << "crc did not match preexisting cache file";
    cache.close();
    return record;
  }

  uint32_t num_entries = ReadAsBytes<uint32_t>(cache);
  BOOST_LOG_TRIVIAL(debug) << "Cache file with " << num_entries << " entries found";
  cache_record temp;
  uint16_t pathlen;
  for (int i = 0; i < num_entries; i++) {
    temp.type = static_cast<CacheType>(ReadAsBytes<uint16_t>(cache));
    pathlen = ReadAsBytes<uint16_t>(cache);
    temp.path.resize(pathlen);
    cache.read(&temp.path[0], pathlen);
    temp.file_size = ReadAsBytes<uint64_t>(cache);
    record.push_back(temp);
  }

  cache.close();
  return record;
}

CachedFileLoader::~CachedFileLoader() {
  // write to the cache :)
  std::fstream cache_output;
  cache_output.open(cache_path_, std::fstream::in | std::fstream::out | std::fstream::trunc | std::fstream::binary);
  std::vector<cache_record> cache;
  // ensure that cache is complete
  std::vector<cache_record> files = file_loader_->GetCache();
  cache.reserve(files.size());
  cache.insert(cache.end(), files.begin(), files.end());

  WriteAsBytes(cache_output, CACHE_MAGIC);
  cache_output.seekp(CACHE_DATA_START - 4, std::ios_base::beg);
  WriteAsBytes(cache_output, static_cast<uint32_t>(cache.size()));
  for (auto entry : cache) {
    WriteAsBytes(cache_output, static_cast<uint16_t>(entry.type));
    WriteAsBytes(cache_output, static_cast<uint16_t>(entry.path.size()));
    cache_output.write(&entry.path[0], entry.path.size());
    WriteAsBytes(cache_output, static_cast<uint64_t>(entry.file_size));
  }

  uint32_t crc = utils::fileutils::CalculateCRCHash(cache_output, CACHE_DATA_START);
  cache_output.seekp(CACHE_DATA_START - 8, std::ios_base::beg);
  WriteAsBytes(cache_output, static_cast<uint32_t>(crc));
  cache_output.close();
}

} // namespace file
} // namespace monkeysworld