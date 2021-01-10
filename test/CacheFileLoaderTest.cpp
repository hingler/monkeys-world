#include <file/CachedFileLoader.hpp>

#include <gtest/gtest.h>

#include <fstream>
#include <iostream>
#include <thread>

using ::monkeysworld::file::CachedFileLoader;

TEST(CacheTests, CreateEmptyCache) {
  remove("resources/cache/testcache.cache");
  CachedFileLoader loader("testcache");
  auto test = loader.LoadFile("resources/themoney.txt");
  auto testtwo = loader.LoadFile("resources/glsl/matte-material/matte-material.frag");
  std::istream worldwide(&test);
}

TEST(CacheTests, ReadExistingCache) {
  remove("resources/cache/existingcache.cache");

  {
    CachedFileLoader loader("existingcache");
    loader.LoadFile("resources/glsl/matte-material/matte-material.vert");
    loader.LoadFile("resources/test/dummy-shader.frag");
    loader.LoadFile("resources/test/dummy-shader.vert");
  }

  std::cout << "Re-reading cache..." << std::endl;
  CachedFileLoader loader("existingcache");
}

// verify contents (should be fine lol)
TEST(CacheTests, ReadCacheContents) {
  remove("resources/cache/coolcache.cache");
  CachedFileLoader loader("coolcache");
  auto test = loader.LoadFile("resources/themoney.txt");
  auto testtwo = loader.LoadFile("resources/glsl/matte-material/matte-material.frag");
  std::istream test_cache(&test);
  std::istream testtwo_cache(&testtwo);
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