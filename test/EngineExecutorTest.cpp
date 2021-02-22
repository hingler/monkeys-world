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

  std::future<bool> res = exec.RunOnMainThread(sample_lambda);
  exec.RunTasks(1);
  ASSERT_TRUE(res.get());
}

TEST(EngineExecutorTests, VerifyRunTime) {
  EngineExecutor exec;
  auto long_task = [&]() -> int {
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    return 1;
  };

  auto short_task = [&]() -> int {
    return 2;
  };

  auto long_res = exec.RunOnMainThread(long_task);
  auto short_res = exec.RunOnMainThread(short_task);

  exec.RunTasks(0.5);
  ASSERT_EQ(long_res.wait_for(std::chrono::milliseconds(1)), std::future_status::ready);
  ASSERT_EQ(short_res.wait_for(std::chrono::milliseconds(1)), std::future_status::timeout);

  ASSERT_EQ(long_res.get(), 1);
}