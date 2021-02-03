#ifndef LOADER_THREAD_POOL_H_
#define LOADER_THREAD_POOL_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace monkeysworld {
namespace file {

/**
 *  A simple thread pool implementation used by our file loaders.
 */ 
class LoaderThreadPool {
 public:
  LoaderThreadPool(int num_threads);

  // method which accepts a task (which is intrinsically tied to a future) and computes it
  // create a promise
  // pass the promise to this
  // return the future contained in that promise

  /**
   *  Adds a new task to the thread pool.
   *  @param task - a lambda which will be run by this thread pool.
   */ 
  void AddTaskToQueue(std::function<void()> func);

  ~LoaderThreadPool();
  LoaderThreadPool& operator=(const LoaderThreadPool& other) = delete;
  LoaderThreadPool(const LoaderThreadPool& other) = delete;

  LoaderThreadPool& operator=(LoaderThreadPool&& other);
  LoaderThreadPool(LoaderThreadPool&& other);
 private:
  // threads are always running
  // threads should also rely on a kill signal telling them to wind down
  // when a thread completes a task, it will check the task queue for something to do
  // if there's nothing there, it should wait on a cond var

  // meanwhile:
  // when we add a task to the queue, notify one thread.
  // that thread will pluck the task off the queue!

  /**
   *  Function used by threads.
   *  @param flag - flag associated with this thread's execution
   */ 
  void threadfunc_(std::atomic_flag* flag);

  int num_threads_;
  std::thread* threads_;                            // array of threads
  std::atomic_flag* flags_;                         // flags for each thread
  std::queue<std::function<void()>> task_queue_;    // queue of tasks to run
  std::mutex queue_lock_;                           // lock for accessing task queue
  std::condition_variable task_condvar_;            // condition var for threads
};

}
}

#endif