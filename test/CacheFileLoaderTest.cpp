#include <file/CachedFileLoader.hpp>

#include <gtest/gtest.h>

using ::screenspacemanager::file::CachedFileLoader;

TEST(CacheTests, CreateEmptyCache) {
  CachedFileLoader loader("testcache");
}