#ifndef BASE_ENGINE_H_
#define BASE_ENGINE_H_

#include <engine/Scene.hpp>
#include <engine/Context.hpp>

namespace monkeysworld {
namespace engine {
namespace baseengine {

// 1. get a window by initializing glfw
// 2. use that window to get a context
// 3. use that window and that context to construct your scene
// 4. pass all three to GameLoop to start running

/**
 *  Initializes GLFW context.
 *  @param win_width - width Gaof result window
 *  @param win_height - height of result window
 *  @returns the window associated with this engine instance.
 * 
 *  TODO: Separate glad/GLFW initialization from window creation
 */  
GLFWwindow* InitializeGLFW(int win_width, int win_height, const std::string& window_name);

/**
 *  Creates a new context object.
 *  @param window - window used by context to set up events
 *  @returns ptr to new context object :)
 */ 
std::shared_ptr<engine::Context> CreateContext(GLFWwindow* window, std::shared_ptr<engine::Scene> scene);
// function to start the engine loop with some premade scene

/**
 *  Function call which starts the main game loop.
 *  @param scene - the first scene to be loaded.
 *  @param ctx - the context associated with this window.
 *  @param window_name - the name associated with this window.
 */ 
void GameLoop(std::shared_ptr<engine::Context> ctx, GLFWwindow* window);

// various functions which help push the engine along

/**
 *  Updates the context in one pass.
 *  @param delta - number of seconds required to render the last frame.
 *  TODO: (this should probably be static :)
 */ 
void UpdateCtx(double delta, engine::Context* ctx);

}
}
}

#endif