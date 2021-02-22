#ifndef ENGINE_EXECUTOR_H_
#define ENGINE_EXECUTOR_H_

#include <engine/Executor.hpp>

#include <future>
#include <mutex>
#include <queue>

namespace monkeysworld {
namespace engine {

class EngineExecutor : public Executor<EngineExecutor> {
 public:
  /**
   *  Constructs a new EngineExecutor
   */ 
  EngineExecutor();

  /**
   *  Implementation for Executor::RunOnMainThread.
   */ 
  template <typename Callable>
  auto RunOnMainThread(Callable func) -> std::future<decltype(func())> {
    using ret_type = decltype(func());
    
    std::shared_ptr<std::promise<ret_type>> p = std::make_shared<std::promise<ret_type>>();
    auto lambda = [=] {
      ret_type res = func();
      p->set_value(res);
    };

    {
      std::unique_lock<std::mutex>(queue_mutex_);
      func_queue_.push(lambda);
    }

    return std::move(p->get_future());
  }

  /**
   *  Grabs tasks from the task queue and runs them continuously, until
   *  either the queue is emptied or the provided time duration passes.
   * 
   *  @param time - amount of time, in ms, which we want to run tasks for at maximum.
   */ 
  void RunTasks(double time);

  EngineExecutor(const EngineExecutor& other) = delete;
  EngineExecutor& operator=(const EngineExecutor& other) = delete;
  EngineExecutor(EngineExecutor&& other) = delete;
  EngineExecutor& operator=(EngineExecutor&& other) = delete;
 private:
  std::mutex queue_mutex_;
  std::queue<std::function<void()>> func_queue_; // queue of functions which will be executed

};

}
}

#endif