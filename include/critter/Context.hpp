#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <file/CachedFileLoader.hpp>
// how best to include camera?

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <critter/Camera.hpp>

#include <memory>

namespace monkeysworld {
namespace critter {

/**
 *  The `Context` class intends to provide access to components for which a single instance will be associated
 *  with execution at a time.
 * 
 *  For instance, handlers for audio, file loaders, global event broadcasters, etc.
 *  All resources here *should* be threadsafe.
 */ 
class Context {
  // ctor, or get/set?
  // store shared ptr
  // other context objects can "get" from last context (probably a good idea to port over like audio handler for inst)
  // use a builder pattern to create the context
    // use premade assets if available, or construct from defaults if not
  // something for borrowing components
  // create a context builder which can be used to pass in components?
  // data class which contains dependencies
  // and which can be fetched from the struct

  // this should make it easier to add components and shit

  // note: the CONTEXT should be detached from state. this thing should exist from start to finish,
  // and should be closely tied to system components which require a rooted
 public:
  /**
   *  Default constructor which attempts to initialize all fields.
   *  @param window - reference to the GLFWwindow this context occupies
   */ 
  Context(GLFWwindow* window);

  // the following functions return some higher level component

  // add timer + timed event handler

  // file loader
  const std::shared_ptr<file::CachedFileLoader> GetCachedFileLoader();

  /**
   *  Returns the size of the framebuffer.
   *  @param width -  output param for width.
   *  @param height - output param for height.
   */  
  void GetFramebufferSize(int* width, int* height);

  /**
   *  Returns the active camera.
   *  @returns ptr to active camera.
   */ 
  Camera* GetActiveCamera();

  // functions to add some object as the root
  // functions to get delta time
  // 
  // functions to find objects in ui/game hierarchy

 private:
  /**
   *  Called by engine loop per-frame to update the context state
   */ 
  void CtxUpdate_();
  std::shared_ptr<file::CachedFileLoader> file_loader_;
  GLFWwindow* window_;
};

} // namespace critter
} // namespace monkeysworld

#endif