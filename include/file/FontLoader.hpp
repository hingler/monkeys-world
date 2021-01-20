#ifndef FONT_LOADER_H_
#define FONT_LOADER_H_

#include <font/Font.hpp>
#include <file/LoaderThreadPool.hpp>
#include <file/CachedLoader.hpp>

#include <condition_variable>
#include <future>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

namespace monkeysworld {
namespace file {

/**
 *  Utility for loading fonts from file.
 *  Since opengl context is limited to a single thread, no async options are avail.
 */ 
class FontLoader : public CachedLoader<std::shared_ptr<font::Font>, FontLoader> {
 public:
  FontLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
             std::vector<cache_record> cache);

  std::shared_ptr<font::Font> LoadFile(const std::string& path);
  std::vector<cache_record> GetCache() override;
  loader_progress GetLoaderProgress() override;
  void WaitUntilLoaded() override;

  bool IsCached(const std::string& path) override;
 private:
  void LoadFontToCache(cache_record& record);

  loader_progress loader_;
  std::mutex loader_mutex_;
  std::shared_timed_mutex cache_mutex_;
  std::unordered_map<std::string, std::shared_ptr<font::Font>> font_cache_;
  std::condition_variable load_cond_var_;
};

}
}

#endif