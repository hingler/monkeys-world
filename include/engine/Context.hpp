#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <file/CachedFileLoader.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <input/WindowEventManager.hpp>
#include <audio/AudioManager.hpp>

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
 */
namespace monkeysworld {
namespace engine {

class Scene;
class Context;

}
}

namespace monkeysworld {
namespace engine {
namespace baseengine {
void UpdateCtx(double, engine::Context*);
}
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
class Context {
  friend void ::monkeysworld::engine::baseengine::UpdateCtx(double, Context*);
 public:
  /**
   *  Creates a context which is associated with the first desired scene.
   *  Scene is initialized in constructor.
   *  @param window - reference to the GLFWwindow this context occupies
   *  @param scene - the first scene associated with this context. The context takes ownership
   *                 of the passed-in scene.
   */ 
  Context(GLFWwindow* window, Scene* scene);

  // the following functions return some higher level component

  // add timer + timed event handler

  // (deprecated)
  const std::shared_ptr<file::CachedFileLoader> GetCachedFileLoader();

  const std::shared_ptr<input::WindowEventManager> GetEventManager();

  const std::shared_ptr<audio::AudioManager> GetAudioManager();

  /**
   *  Returns the size of the framebuffer.
   *  @param width -  output param for width.
   *  @param height - output param for height.
   */  
  void GetFramebufferSize(int* width, int* height);

  /**
   *  Notifies the context that it should swap to the next scene
   */ 
  void SwapScene(Scene* scene);

  /**
   *  @returns the currently displayed scene.
   */ 
  Scene* GetScene();

  double GetDeltaTime();

  ~Context();

 private:
  std::shared_ptr<file::CachedFileLoader> file_loader_;
  std::shared_ptr<input::WindowEventManager> event_mgr_;
  std::shared_ptr<audio::AudioManager> audio_mgr_;
  Scene* scene_;
  GLFWwindow* window_;
  // the current scene
  double frame_delta_;

 protected:
  /**
   *  Calls all functions necessary when updating to the next frame.
   *  Thus far, this includes:
   *  - Processing any waiting events. 
   */ 
  void FrameUpdate();
};

} // namespace critter
} // namespace monkeysworld

#endif