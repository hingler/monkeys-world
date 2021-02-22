#include <engine/EngineExecutor.hpp>

#include <chrono>

namespace monkeysworld {
namespace engine {

EngineExecutor::EngineExecutor() {}

void EngineExecutor::RunTasks(double time) {
  auto start = std::chrono::high_resolution_clock::now();
  auto end = start;
  std::chrono::duration<double, std::milli> dur = end - start;
  
  bool read;
  {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    read = !func_queue_.empty();
  }

  while (read && dur.count() < time) {
    std::function<void()> func;
    {
      std::unique_lock<std::mutex> lock(queue_mutex_);
      func = func_queue_.front();
      func_queue_.pop();
      read = !func_queue_.empty();
    }

    func();
    end = std::chrono::high_resolution_clock::now();
    dur = end - start;
  }
}

}
}