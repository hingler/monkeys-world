#include <file/CachedFileLoader.hpp>

#include <gtest/gtest.h>

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
  {
    CachedFileLoader loader("existingcache");
    loader.LoadFile("resources/glsl/matte-material/matte-material.vert");
    loader.LoadFile("resources/test/dummy-shader.frag");
    loader.LoadFile("resources/test/dummy-shader.vert");
  }

  CachedFileLoader loader("existingcache");
}