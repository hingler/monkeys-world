#ifndef ENGINE_CONTEXT_H_
#define ENGINE_CONTEXT_H_

#include <file/CachedFileLoader.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <engine/Context.hpp>
#include <engine/SceneSwap.hpp>
#include <input/WindowEventManager.hpp>
#include <audio/AudioManager.hpp>
#include <shader/Texture.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

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

  /**
   *  Initializes the scene associated with this context.
   */ 
  void InitializeScene();

  // (deprecated)
  std::shared_ptr<file::CachedFileLoader> GetCachedFileLoader() override;

  std::shared_ptr<input::WindowEventManager> GetEventManager() override;

  std::shared_ptr<audio::AudioManager> GetAudioManager() override;

  std::shared_ptr<Executor<EngineExecutor>> GetExecutor() override;

  std::shared_ptr<shader::Texture> GetLastFrame() override;
  
  void GetFramebufferSize(int* width, int* height) override;

  std::shared_ptr<SceneSwap> SwapScene(Scene* scene) override;

  Scene* GetScene() override;

  double GetDeltaTime() override;

  /**
   *  If this context is ready to give up control, this call will return a pointer to the new context.
   *  Otherwise, returns null.
   */ 
  std::shared_ptr<EngineContext> GetNewContext();

  /**
   *  Notifies the context to update itself.
   */ 
  void UpdateContext();

  // copy ctor -- used to initialize a new enginecontext from the old one.
  EngineContext(const EngineContext& other, Scene* scene);
  
  ~EngineContext();

 private:
  std::shared_ptr<file::CachedFileLoader> file_loader_;
  std::shared_ptr<input::WindowEventManager> event_mgr_;
  std::shared_ptr<audio::AudioManager> audio_mgr_;
  std::shared_ptr<EngineExecutor> executor_;
  Scene* scene_;
  GLFWwindow* window_;
  // the current scene
  std::atomic<double> frame_delta_;
  std::chrono::time_point<std::chrono::high_resolution_clock> start_;
  std::chrono::time_point<std::chrono::high_resolution_clock> finish_;
  std::chrono::duration<double, std::ratio<1, 1>> dur_;

  // new scene loading
  // our load thread will wait for the context to load
  // once it does, it will
  std::thread swap_thread_;
  std::shared_ptr<SceneSwap> swap_obj_;
  std::shared_ptr<EngineContext> swap_ctx_;
  std::shared_ptr<std::condition_variable> swap_cv_;
  std::shared_ptr<std::mutex> swap_mutex_;

  // stores the result of the last frame
  std::shared_ptr<shader::Texture> last_frame_;
  glm::ivec2 last_frame_dims_;

  

  bool initialized_;
  
};

} // namespace critter
} // namespace monkeysworld

#endif