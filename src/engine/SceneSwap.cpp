#include <engine/SceneSwap.hpp>
#include <engine/Context.hpp>

#include <engine/Scene.hpp>

namespace monkeysworld {
namespace engine {

SceneSwap::SceneSwap(std::shared_ptr<Context> ctx,
                     std::shared_ptr<std::mutex> mutex,
                     std::shared_ptr<std::condition_variable> load_cv) {
  // ok
  ctx_ = ctx;
  mutex_ = mutex;
  load_cv_ = load_cv;
  swap_ready_ = false;
}

file::loader_progress SceneSwap::GetLoaderProgress() {
  return ctx_->GetCachedFileLoader()->GetLoaderProgress();
}

bool SceneSwap::Initialized() {
  return ctx_->GetScene()->IsInitialized();
}

void SceneSwap::Swap() {
  if (!swap_ready_) {
    std::unique_lock<std::mutex> lock(*mutex_);
    load_cv_->wait(lock, [&] {
      return ctx_->GetScene()->IsInitialized();
    });

    swap_ready_.store(true, std::memory_order_seq_cst);
  }
}

bool SceneSwap::IsSwapReady() {
  return swap_ready_.load(std::memory_order_seq_cst);
}

}
}