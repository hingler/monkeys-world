#include <file/LoaderThreadPool.hpp>
#include <gtest/gtest.h>

using ::monkeysworld::file::LoaderThreadPool;

TEST(LoaderThreadPoolTests, CreateThreadPool) {
  int test = 0;
  
  {
    LoaderThreadPool pool(4);
    auto lambda = [&]() {
      test++;
    };

    pool.AddTaskToQueue(lambda);

    // this implies a race on tasks. should be a better way to wait for empty, then destroy.
    // i'll stick with the crappy ver for now, and if it becomes a problem, add to the spec.

    // sample impl uses a "stop" atomic to tell the whole thread pool to stop accepting commands
  }

  ASSERT_EQ(1, test);
}

TEST(LoaderThreadPoolTests, MultipleTasks) {
  std::atomic<int> test = 0;

  {
    LoaderThreadPool pool(8);
    auto lambda = [&]() {
      test.fetch_add(1);
    };

    for (int i = 0; i < 512; i++) {
      pool.AddTaskToQueue(lambda);
    }
  }

  ASSERT_EQ(512, test.load());
}