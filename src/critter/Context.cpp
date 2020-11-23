#include <critter/Context.hpp>

namespace monkeysworld {
namespace critter {

using file::CachedFileLoader;

Context::Context() {
  file_loader_ = std::make_shared<CachedFileLoader>("context_cache");
}

const std::shared_ptr<CachedFileLoader> Context::GetCachedFileLoader() {
  return file_loader_;
}

} // namespace critter
} // namespace monkeysworld