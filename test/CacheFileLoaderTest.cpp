#include <file/CachedFileLoader.hpp>

#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <thread>

using ::screenspacemanager::file::CachedFileLoader;

TEST(CacheTests, CreateEmptyCache) {
  remove("resources/cache/testcache.filecache");
  CachedFileLoader loader("testcache");
  std::unique_ptr<std::streambuf> test = loader.LoadFile("resources/themoney.txt");
  std::unique_ptr<std::streambuf> testtwo = loader.LoadFile("resources/glsl/matte-material/matte-material.frag");
  std::istream worldwide(test.get());
}

TEST(CacheTests, ReadExistingCache) {
  remove("resources/cache/existingcache.filecache");

  {
    CachedFileLoader loader("existingcache");
    loader.LoadFile("resources/glsl/matte-material/matte-material.vert");
    loader.LoadFile("resources/test/dummy-shader.frag");
    loader.LoadFile("resources/test/dummy-shader.vert");
  }

  CachedFileLoader loader("existingcache");
}

// verify contents (should be fine lol)
TEST(CacheTests, ReadCacheContents) {
  remove("resources/cache/coolcache.filecache");
  CachedFileLoader loader("coolcache");
  std::unique_ptr<std::streambuf> test = loader.LoadFile("resources/themoney.txt");
  std::unique_ptr<std::streambuf> testtwo = loader.LoadFile("resources/glsl/matte-material/matte-material.frag");
  std::istream test_cache(test.get());
  std::istream testtwo_cache(testtwo.get());
  std::ifstream test_verify("resources/themoney.txt", std::ifstream::in | std::ifstream::binary);
  std::ifstream testtwo_verify("resources/glsl/matte-material/matte-material.frag", std::ifstream::in | std::ifstream::binary);
  while (!test_cache.eof()) {
    ASSERT_EQ(test_verify.get(), test_cache.get());
  }
  uint32_t i = 0;
  while (!testtwo_cache.eof()) {
    ASSERT_EQ(testtwo_verify.get(), testtwo_cache.get());
  }
}