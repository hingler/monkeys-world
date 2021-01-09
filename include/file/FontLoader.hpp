#ifndef FONT_LOADER_H_
#define FONT_LOADER_H_

#include <font/Font.hpp>
#include <file/LoaderThreadPool.hpp>
#include <file/CachedLoader.hpp>

#include <future>
#include <memory>
#include <mutex>
#include <shared_mutex>

namespace monkeysworld {
namespace file {

/**
 *  Utility for loading fonts from file.
 *  Since opengl context is limited to a single thread, no async options are avail.
 */ 
class FontLoader : public CachedLoader {
 public:
  FontLoader(std::shared_ptr<LoaderThreadPool> thread_pool,
             std::vector<cache_record> cache);

  /**
   *  Synchronously loads a font.
   */ 
  std::shared_ptr<font::Font> LoadFont(const std::string& path);

  /**
   *  ASYNCHRONOUSLY loads the font >:)
   */ 
  std::future<std::shared_ptr<font::Font>> LoadFontAsync(const std::string& path);

  std::vector<cache_record> GetCache() override;
  loader_progress GetLoaderProgress() override;
 private:

  std::shared_ptr<font::Font> LoadFontFromFile(const std::string& path);
  void LoadFontToCache(cache_record& record);

  loader_progress loader_;
  std::mutex loader_mutex_;
  std::shared_timed_mutex cache_mutex_;
  std::unordered_map<std::string, std::shared_ptr<font::Font>> font_cache_;
  std::shared_ptr<LoaderThreadPool> thread_pool_;
};

}
}

#endif