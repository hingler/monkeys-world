#include <engine/EngineExecutor.hpp>

#include <chrono>

namespace monkeysworld {
namespace engine {

EngineExecutor::EngineExecutor() {
  main_thread_id_ = std::this_thread::get_id();
}

void EngineExecutor::RunTasks(double time) {
  auto start = std::chrono::high_resolution_clock::now();
  auto end = start;
  std::chrono::duration<double, std::ratio<1L, 1L>> dur = end - start;
  
  bool read;
  std::unique_lock<std::mutex> lock(queue_mutex_);
  read = !func_queue_.empty();

  while (read && dur.count() < time) {
    std::function<void()> func;
    func = func_queue_.front();
    func_queue_.pop();
    read = !func_queue_.empty();

    func();
    end = std::chrono::high_resolution_clock::now();
    dur = end - start;
  }
}

}
}