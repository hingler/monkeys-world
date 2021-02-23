#ifndef EXECUTOR_H_
#define EXECUTOR_H_

#include <functional>
#include <future>
#include <thread>

namespace monkeysworld {
namespace engine {

// allows us to use the EngineExecutor as a default
class EngineExecutor;

/**
 *  The executor class enables the client to guarantee that certain functions are run
 *  on the main thread. This can be useful in cases in which the desired function call
 *  may not be guaranteed to run on the main thread, for instance in constructors.
 */ 
template <typename Derived = EngineExecutor>
class Executor {
 public:
  /**
   *  Runs a particular function on the main thread, and returns the value that function returns.
   *  @param func - the function which will be run.
   *  @param Callable - a callable argument.
   *  @returns - a future which will resolve to the result of the function call.
   */ 
  template <typename Callable>
  auto ReturnOnMainThread(Callable func) -> std::future<decltype(func())> {
    Derived* that = static_cast<Derived*>(this);
    return that->ReturnOnMainThread(func);
  }

  std::future<void> ScheduleOnMainThread(std::function<void()> func) {
    Derived* that = static_cast<Derived*>(this);
    return that->ScheduleOnMainThread(func);
  }

};

}
}

#endif