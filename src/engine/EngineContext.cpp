#include <engine/EngineContext.hpp>
#include <engine/Scene.hpp>
#include <engine/SceneSwap.hpp>

namespace monkeysworld {
namespace engine {

using file::CachedFileLoader;
using input::WindowEventManager;
using audio::AudioManager;

EngineContext::EngineContext(GLFWwindow* window, Scene* scene) {
  file_loader_ = std::make_shared<CachedFileLoader>(scene->GetSceneIdentifier());
  event_mgr_ = std::make_shared<WindowEventManager>(window, this);
  audio_mgr_ = std::make_shared<AudioManager>();
  executor_ = std::make_shared<EngineExecutor>();

  window_ = window;

  start_ = std::chrono::high_resolution_clock::now();

  swap_ctx_ = nullptr;
  swap_cv_ = std::make_shared<std::condition_variable>();
  swap_mutex_ = std::make_shared<std::mutex>();

  scene_ = scene;
  initialized_ = false;
}

void EngineContext::InitializeScene() {
  if (!initialized_) {
    scene_->CreateScene(this);
  }

  initialized_ = true;
}

void EngineContext::GetFramebufferSize(int* width, int* height) {
  glfwGetFramebufferSize(window_, width, height);
}

std::shared_ptr<CachedFileLoader> EngineContext::GetCachedFileLoader() {
  return file_loader_;
}

std::shared_ptr<WindowEventManager> EngineContext::GetEventManager() {
  return event_mgr_;
}

std::shared_ptr<AudioManager> EngineContext::GetAudioManager() {
  return audio_mgr_;
}

std::shared_ptr<Executor<>> EngineContext::GetExecutor() {
  return executor_;
}

Scene* EngineContext::GetScene() {
  return scene_;
}

std::shared_ptr<SceneSwap> EngineContext::SwapScene(Scene* scene) {
  // ensure that the context is created before we create the sceneswap obj
  
  swap_ctx_ = std::make_shared<EngineContext>(*this, scene);
  swap_obj_ = std::make_shared<SceneSwap>(swap_ctx_, swap_mutex_, swap_cv_);
  return swap_obj_;
}

std::shared_ptr<EngineContext> EngineContext::GetNewContext() {
  if (!swap_ctx_) {
    return nullptr;
  }

  auto prog = swap_ctx_->GetCachedFileLoader()->GetLoaderProgress();
  if (prog.bytes_read == prog.bytes_sum) {
    // notify scene swap object if we're ready
    swap_cv_->notify_all();
    if (swap_obj_->IsSwapReady()) {
      // if it's ready, then return the new ctx
      return swap_ctx_;
    }
  }

  return nullptr;
}

double EngineContext::GetDeltaTime() {
  return frame_delta_.load();
}

void EngineContext::UpdateContext() {
  finish_ = std::chrono::high_resolution_clock::now();
  dur_ = (finish_ - start_);
  frame_delta_.store(dur_.count());
  start_ = finish_;
  event_mgr_->ProcessWaitingEvents();
  // ~10ms
  executor_->RunTasks(0.010);
}

EngineContext::~EngineContext() {
  delete scene_;
}

EngineContext::EngineContext(const EngineContext& other, Scene* scene) {
  file_loader_ = std::make_shared<CachedFileLoader>(scene->GetSceneIdentifier());
  event_mgr_ = other.event_mgr_;
  audio_mgr_ = other.audio_mgr_;
  window_ = other.window_;

  start_ = std::chrono::high_resolution_clock::now();

  swap_ctx_ = nullptr;
  swap_cv_ = std::make_shared<std::condition_variable>();
  swap_mutex_ = std::make_shared<std::mutex>();

  scene_ = scene;
}


} // namespace critter
} // namespace monkeysworld