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

void SceneSwap::Swap() {
  std::unique_lock<std::mutex> lock(*mutex_);
  load_cv_->wait(lock, [&] {
    return ctx_->GetScene()->IsInitialized();
  });

  swap_ready_ = true;
}

bool SceneSwap::IsSwapReady() {
  return swap_ready_;
}

}
}