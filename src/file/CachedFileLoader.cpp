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
  model_loader_ = std::make_unique<ModelLoader>(thread_pool_, cache);
  font_loader_ = std::make_unique<FontLoader>(thread_pool_, cache);
}

loader_progress CachedFileLoader::GetLoaderProgress() {
  loader_progress res;
  loader_progress temp;

  res.bytes_read = 0;
  res.bytes_sum = 0;

  temp = file_loader_->GetLoaderProgress();
  res.bytes_read += temp.bytes_read;
  res.bytes_sum += temp.bytes_sum;

  temp = model_loader_->GetLoaderProgress();
  res.bytes_read += temp.bytes_read;
  res.bytes_sum += temp.bytes_sum;

  temp = font_loader_->GetLoaderProgress();
  res.bytes_read += temp.bytes_read;
  res.bytes_sum += temp.bytes_sum;

  return res;
}

CacheStreambuf CachedFileLoader::LoadFile(const std::string& path) {
  return file_loader_->LoadFile(path);
}

std::shared_ptr<const model::Mesh<storage::VertexPacket3D>> CachedFileLoader::LoadModel(const std::string& path) {
  return model_loader_->LoadFile(path);
}

std::shared_ptr<const font::Font> CachedFileLoader::LoadFont(const std::string& path) {
  return font_loader_->LoadFile(path);
}

std::vector<cache_record> CachedFileLoader::ReadCacheFileToVector(const std::string& cache_path) {
  std::vector<cache_record> record;
  std::ifstream cache(cache_path, std::ios_base::in | std::ios_base::binary);
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
  cache.seekg(CACHE_DATA_START, std::ios_base::beg);
  uint32_t crc_actual = utils::fileutils::CalculateCRCHash(cache, std::streampos(CACHE_DATA_START));
  if (crc_expected != crc_actual) {
    BOOST_LOG_TRIVIAL(warning) << "crc did not match preexisting cache file -- expected " << crc_expected << ", actual " << crc_actual;
    cache.close();
    return record;
  }

  cache.seekg(CACHE_DATA_START - 4, std::ios_base::beg);
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
  file_loader_->WaitUntilLoaded();
  model_loader_->WaitUntilLoaded();
  font_loader_->WaitUntilLoaded();
  std::fstream cache_output;
  cache_output.open(cache_path_, std::fstream::in | std::fstream::out | std::fstream::trunc | std::fstream::binary);
  std::vector<cache_record> cache;
  // ensure that cache is complete
  std::vector<cache_record> files = file_loader_->GetCache();
  std::vector<cache_record> meshes = model_loader_->GetCache();
  std::vector<cache_record> fonts = font_loader_->GetCache();
  cache.reserve(files.size() + meshes.size() + fonts.size());
  cache.insert(cache.end(), files.begin(), files.end());
  cache.insert(cache.end(), meshes.begin(), meshes.end());
  cache.insert(cache.end(), fonts.begin(), fonts.end());
  BOOST_LOG_TRIVIAL(debug) << "cacheing " << cache.size() << " files";

  cache_output.seekp(0);
  WriteAsBytes(cache_output, CACHE_MAGIC);
  cache_output.seekp(CACHE_DATA_START);
  for (auto entry : cache) {
    WriteAsBytes(cache_output, static_cast<uint16_t>(entry.type));
    WriteAsBytes(cache_output, static_cast<uint16_t>(entry.path.size()));
    cache_output.write(&entry.path[0], entry.path.size());
    WriteAsBytes(cache_output, static_cast<uint64_t>(entry.file_size));
  }

  uint32_t crc = utils::fileutils::CalculateCRCHash(cache_output, std::streampos(CACHE_DATA_START));
  cache_output.seekp(CACHE_DATA_START - 8);
  WriteAsBytes(cache_output, static_cast<uint32_t>(crc));
  BOOST_LOG_TRIVIAL(trace) << "CRC: " << crc;
  WriteAsBytes(cache_output, static_cast<uint32_t>(cache.size()));
  cache_output.close();
}

} // namespace file
} // namespace monkeysworld