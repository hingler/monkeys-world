#ifndef ENGINE_CONTEXT_H_
#define ENGINE_CONTEXT_H_

#include <file/CachedFileLoader.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <engine/Context.hpp>
#include <input/WindowEventManager.hpp>
#include <audio/AudioManager.hpp>

#include <atomic>
#include <chrono>
#include <memory>

/**
 *  TODO (while i remember it): if we want to alter vertex positions in shader, pass it to the shadow renderer
 * 
 *  Unity abstracts this via surface shaders which handle the crap
 *  But if you write your own, you have some param representing a tag which
 *  determines which pass you're rendering, and then the implementor is required to return
 *  the proper data from that shader.
 * 
 *  This would be an option later down the line, but for now i'm just going to keep it simple
 *  and handle anything like that in CPU
 * 
 *  TODO2: should this be labeled in some way as an internal-only class?
 */
namespace monkeysworld {
namespace engine {

class Context;

}
}

namespace monkeysworld {
namespace engine {


/**
 *  The `Context` class intends to provide access to components for which a single instance will be associated
 *  with execution at a time.
 * 
 *  For instance, handlers for audio, file loaders, global event broadcasters, etc.
 *  All resources here *should* be threadsafe.
 */ 
class EngineContext : public Context {
 public:
  /**
   *  Creates a context which is associated with the first desired scene.
   *  Scene is initialized in constructor.
   *  @param window - reference to the GLFWwindow this context occupies
   *  @param scene - the first scene associated with this context. The context takes ownership
   *                 of the passed-in scene.
   */ 
  EngineContext(GLFWwindow* window, Scene* scene);

  // the following functions return some higher level component

  // add timer + timed event handler

  // (deprecated)
  std::shared_ptr<file::CachedFileLoader> GetCachedFileLoader() override;

  std::shared_ptr<input::WindowEventManager> GetEventManager() override;

  std::shared_ptr<audio::AudioManager> GetAudioManager() override;
  
  void GetFramebufferSize(int* width, int* height) override;

  void SwapScene(Scene* scene) override;

  Scene* GetScene() override;

  double GetDeltaTime() override;

  /**
   *  Notifies the context to update itself.
   */ 
  void UpdateContext();

  ~EngineContext();

 private:
  std::shared_ptr<file::CachedFileLoader> file_loader_;
  std::shared_ptr<input::WindowEventManager> event_mgr_;
  std::shared_ptr<audio::AudioManager> audio_mgr_;
  Scene* scene_;
  GLFWwindow* window_;
  // the current scene
  std::atomic<double> frame_delta_;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
  std::chrono::time_point<std::chrono::high_resolution_clock> finish_;
  std::chrono::duration<double, std::ratio<1, 1>> dur_;
};

} // namespace critter
} // namespace monkeysworld

#endif