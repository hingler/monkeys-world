#include <file/TextureLoader.hpp>
#include <gtest/gtest.h>

using ::monkeysworld::file::TextureLoader;
using ::monkeysworld::file::LoaderThreadPool;
using ::monkeysworld::file::cache_record;

TEST(TextureLoaderTests, CreateTextureLoader) {
  auto threadpool = std::make_shared<LoaderThreadPool>(4);
  TextureLoader tl(threadpool, std::vector<cache_record>());
  auto res = tl.LoadFile("resources/test/texturetest.png");
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  ASSERT_NE(nullptr, res.get());
  ASSERT_LT(100 * 100 * 4, res->GetTextureSize());
}

TEST(TextureLoaderTests, TestAsync) {
  auto threadpool = std::make_shared<LoaderThreadPool>(4);
  TextureLoader tl(threadpool, std::vector<cache_record>());
  auto future = tl.LoadFileAsync("resources/test/texturetest.png");
  std::this_thread::sleep_for(std::chrono::milliseconds(150));
  future.wait();
  auto res = future.get();
  ASSERT_NE(nullptr, res.get());
  ASSERT_LT(100 * 100 * 4, res->GetTextureSize());
}