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
   *  Implementation for Executor::ReturnOnMainThread.
   */ 
  template <typename Callable>
  auto ReturnOnMainThread(Callable func) -> std::future<decltype(func())> {
    using ret_type = decltype(func());
    
    std::shared_ptr<std::promise<ret_type>> p = std::make_shared<std::promise<ret_type>>();
    if (std::this_thread::get_id() == main_thread_id_) {
      // currently on main thread -- don't schedule, just call it.
      p->set_value(func());
    } else {
      std::function<void()> lambda = [=] {
        ret_type res = func();
        p->set_value(res);
      };

      std::unique_lock<std::mutex> lock(queue_mutex_);
      func_queue_.push(lambda);
    }
    
    return std::move(p->get_future());
  }

  std::future<void> ScheduleOnMainThread(std::function<void()> func) {
    std::shared_ptr<std::promise<void>> p = std::make_shared<std::promise<void>>();
    if (std::this_thread::get_id() == main_thread_id_) {
      func();
      p->set_value();
    } else {
      std::function<void()> lambda = [=] {
        func();
        p->set_value();
      };

      std::unique_lock<std::mutex> lock(queue_mutex_);
      func_queue_.push(lambda);
    }

    return std::move(p->get_future());
  }

  // todo: create a version which does not have to return a value :)

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
  std::thread::id main_thread_id_;
};

}
}

#endif