#include <engine/Executor.hpp>
#include <engine/EngineExecutor.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

using ::monkeysworld::engine::EngineExecutor;

TEST(EngineExecutorTests, CreateExecutor) {
  EngineExecutor exec;
  auto sample_lambda = [&]() -> bool {
    return true;
  };

  std::future<bool> res = exec.ReturnOnMainThread(sample_lambda);
  exec.RunTasks(1);
  ASSERT_TRUE(res.get());
}

// this should fail
TEST(EngineExecutorTests, VerifyMainThreadRunsImmediately) {
  EngineExecutor exec;
  auto long_task = [&]() -> int {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return 1;
  };

  auto short_task = [&]() -> int {
    return 2;
  };

  // on a separate thread, only long_res will return, as the alloted time will expire befor
  // short_task can run
  auto long_res = exec.ReturnOnMainThread(long_task);
  auto short_res = exec.ReturnOnMainThread(short_task);

  exec.RunTasks(0.01);
  ASSERT_EQ(long_res.wait_for(std::chrono::milliseconds(1)), std::future_status::ready);
  ASSERT_EQ(short_res.wait_for(std::chrono::milliseconds(1)), std::future_status::ready);

  ASSERT_EQ(long_res.get(), 1);
  ASSERT_EQ(short_res.get(), 2);
}

void queue_tests(std::function<int()> t1, EngineExecutor* exec, std::future<int>* ret) {
  *ret = exec->ReturnOnMainThread(t1);
}

TEST(EngineExecutorTests, RunTestsFromOtherThread) {
  EngineExecutor exec;
  auto long_task = [&]() -> int {
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return 1;
  };

  auto short_task = [&]() -> int {
    return 2;
  };

  std::future<int> f1, f2;

  std::thread t1(&queue_tests, long_task, &exec, &f1);
  t1.join();
  std::thread t2(&queue_tests, short_task, &exec, &f2);
  t2.join();

  exec.RunTasks(0.01);
  ASSERT_EQ(f1.wait_for(std::chrono::milliseconds(1)), std::future_status::ready);
  ASSERT_EQ(f2.wait_for(std::chrono::milliseconds(1)), std::future_status::timeout);

  ASSERT_EQ(f1.get(), 1);
}



TEST(EngineExecutorTests, MultiThreadTests) {
  std::thread threads[16];
  std::future<int> futures[16];

  EngineExecutor exec;

  for (int i = 0; i < 16; i++) {
    threads[i] = std::thread(&queue_tests, [=]() -> int { 
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
      return i; 
    }, &exec, futures + i);
  }

  for (int i = 0; i < 16; i++) {
    threads[i].join();
  }

  exec.RunTasks(1);

  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(futures[i].get(), i);
  }
}