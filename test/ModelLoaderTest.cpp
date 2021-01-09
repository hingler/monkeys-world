#include <file/ModelLoader.hpp>
#include <gtest/gtest.h>

using ::monkeysworld::file::ModelLoader;
using ::monkeysworld::file::LoaderThreadPool;

TEST(ModelLoaderTests, CreateModelLoader) {
  auto threadpool = std::make_shared<LoaderThreadPool>(4);
  ModelLoader m(threadpool, std::vector<::monkeysworld::file::cache_record>());
  auto res = m.LoadOBJ("resources/test/untitled4.obj");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  ASSERT_NE(nullptr, res.get());
  ASSERT_EQ(47194, res->GetVertexCount());
}

TEST(ModelLoaderTests, TestAsync) {
  auto threadpool = std::make_shared<LoaderThreadPool>(4);
  ModelLoader m(threadpool, std::vector<::monkeysworld::file::cache_record>());
  auto future = m.LoadOBJAsync("resources/test/untitled4.obj");
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  future.wait();
  auto res = future.get();
  ASSERT_NE(nullptr, res.get());
  ASSERT_EQ(47194, res->GetVertexCount());
}