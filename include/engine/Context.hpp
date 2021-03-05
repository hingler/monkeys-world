#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <file/CachedFileLoader.hpp>
#include <input/WindowEventManager.hpp>
#include <audio/AudioManager.hpp>
#include <engine/SceneSwap.hpp>
#include <engine/Executor.hpp>
#include <engine/EngineExecutor.hpp>
#include <shader/Framebuffer.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
namespace monkeysworld {
namespace engine {

class Scene;

class Context {

 public:
  virtual std::shared_ptr<file::CachedFileLoader> GetCachedFileLoader() = 0;
  virtual std::shared_ptr<input::EventManager> GetEventManager() = 0;
  virtual std::shared_ptr<audio::AudioManager> GetAudioManager() = 0;
  virtual std::shared_ptr<Executor<EngineExecutor>> GetExecutor() = 0;

  /**
   *  @returns the last rendered frame, as a framebuffer object.
   */ 
  virtual std::shared_ptr<shader::Framebuffer> GetLastFrame() = 0;

  /**
   *  Returns the size of the framebuffer.
   *  @param width -  output param for width.
   *  @param height - output param for height.
   */
  virtual void GetFramebufferSize(int* width, int* height) = 0;

  /**
   *  Notifies the context that it should swap to the next scene.
   *  Once this function is called, the context takes ownership of the scene.
   *  @param scene - the scene which is to be created.
   *                 once called, the context will call scene->Initialize().
   */ 
  virtual std::shared_ptr<SceneSwap> SwapScene(Scene* scene) = 0;

  /**
   *  @returns the currently displayed scene.
   */ 
  virtual Scene* GetScene() = 0;
  virtual double GetDeltaTime() = 0;
  virtual ~Context() {}
};

}
}

#endif