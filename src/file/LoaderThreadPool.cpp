#include <file/LoaderThreadPool.hpp>

namespace monkeysworld {
namespace file {

LoaderThreadPool::LoaderThreadPool(int num_threads) {
  threads_ = new std::thread[num_threads];
  flags_ = new std::atomic_flag[num_threads];
  num_threads_ = num_threads;
  for (int i = 0; i < num_threads_; i++) {
    threads_[i] = std::thread(&LoaderThreadPool::threadfunc_, this, flags_ + i);
  }
}

void LoaderThreadPool::AddTaskToQueue(std::function<void()> func) {
  std::unique_lock<std::mutex>(queue_lock_);
  task_queue_.push(func);
  task_condvar_.notify_all();
}

void LoaderThreadPool::threadfunc_(std::atomic_flag* flag) {
  std::function<void()> task;
  for (;;) {
    {
      std::unique_lock<std::mutex> queue_lock(queue_lock_);
      while (task_queue_.empty()) {
        if (!flag->test_and_set()) {
          return;
        }

        task_condvar_.wait(queue_lock);
      }

      task = task_queue_.front();
      task_queue_.pop();
    }

    task();
  }
}

LoaderThreadPool::~LoaderThreadPool() {
  if (num_threads_ > 0) {
    for (int i = 0; i < num_threads_; i++) {
      // notify all threads to wind down
      flags_[i].clear();
    }

    // start up the threads
    task_condvar_.notify_all();
    for (int i = 0; i < num_threads_; i++) {
      // join as they wind down
      threads_[i].join();
    }

    // delete heap allocated threads and flags
    delete[] threads_;
    delete[] flags_;
  }
}

}
}