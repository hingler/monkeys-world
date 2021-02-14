#include <engine/EngineContext.hpp>
#include <engine/Scene.hpp>

namespace monkeysworld {
namespace engine {

using file::CachedFileLoader;
using input::WindowEventManager;
using audio::AudioManager;

EngineContext::EngineContext(GLFWwindow* window, Scene* scene) {
  file_loader_ = std::make_shared<CachedFileLoader>("context_cache");
  event_mgr_ = std::make_shared<WindowEventManager>(window, this);
  audio_mgr_ = std::make_shared<AudioManager>();
  window_ = window;

  start_ = std::chrono::high_resolution_clock::now();

  scene_ = scene;
  scene_->Initialize(this);
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

Scene* EngineContext::GetScene() {
  return scene_;
}

void EngineContext::SwapScene(Scene* scene) {
  // create a new context which will handle the new scene
  // return some new object which lets the current scene monitor how far we've loaded
  // once completely loaded, and the client gives the OK:
  //  - expose the new context to the engine
  //  - on the next tick, the engine will pick it up and delete this context
  //  - we're done!
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
}

EngineContext::~EngineContext() {
  delete scene_;
}


} // namespace critter
} // namespace monkeysworld