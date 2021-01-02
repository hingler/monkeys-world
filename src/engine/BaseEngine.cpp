#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine/BaseEngine.hpp>

#include <critter/visitor/LightVisitor.hpp>

// TODO: create an actual logging setup -- we can config it in init :)
#include <boost/log/trivial.hpp>

#include <chrono>


namespace monkeysworld {
namespace engine {
namespace baseengine {

using ::monkeysworld::critter::visitor::LightVisitor;
using ::monkeysworld::shader::Material;
using ::monkeysworld::shader::light::SpotLight;
using ::monkeysworld::engine::RenderContext;

static void UpdateObjects(std::shared_ptr<critter::Object>);
// subtype context to enable access to frequent update functions
// pass supertype to scene
void GameLoop(std::shared_ptr<Scene> scene, std::shared_ptr<critter::Context> ctx, GLFWwindow* window) {
  // initialize the scene
  scene->Initialize();
  // setup timing
  auto start = std::chrono::high_resolution_clock::now();
  auto finish = std::chrono::high_resolution_clock::now();
  std::shared_ptr<Scene> current_scene = scene;
  std::chrono::duration<double, std::ratio<1, 1>> dur = finish - start;

  LightVisitor light_visitor;


  

  for (;;) {
    // reset any visitors which store info
    light_visitor.Clear();
    start = std::chrono::high_resolution_clock::now();
    // poll for events
    // check to see if a new scene needs to be initialized
    // if so: do that
    if (ctx->GetNextScene()) {
      // get ready to swap the scene
    }

    UpdateCtx(dur.count(), ctx.get());

    // process all incoming inputs
    // use a friend func to update some constants inherent to the context
    //   ex. the frame delta
    // visit objects in our component tree and call their "update" funcs
    UpdateObjects(current_scene->GetGameObjectRoot());
    current_scene->GetGameObjectRoot()->Accept(light_visitor);
    // for each light:
    //   - do a depth render from the perspective of our lights

    // for now: put the lights by themselves in our render context
    for (auto light : light_visitor.GetSpotLights()) {
      // no render context needed -- we're just preparing attributes and calling a default shadow func
      // we want to have the shadow map shader prepared beforehand

      // create a simple visitor which will pass this light's properties to the program and render to the map
      // lastly: add the light to the render context
    }
    // lights should be able to generate some packet which the renderer can use
    // render objects, using the render context
    // swap buffers

    // SHADOW PASS -- get shadow info
    // add light info to the render context, in appropriate places

    // only spotlights for now -- the goal:
    //   - see which lights are in the scene
    //   - run the light pass for each one
    //   - note: there should be a method which contains all GL calls required to draw the object
    //   - that way, we can separate material configuration from drawing
    //   - add a method to Object which accomplishes this

    // then: just draw (for now)
    //    if needed, we can start to collect additional information and add it to the context
    //    however -- for now, just this.

    // create render context
    // add lights to the render context
    // then: render!
    glfwSwapBuffers(window);
    // loop back
    glfwPollEvents();
  }
}

void UpdateObjects(std::shared_ptr<critter::Object> obj) {
  // call update func
  obj->Update();
  for (auto child : obj->GetChildren()) {
    // if child exists, visit it and queue up its children
    // TODO: write a visitor, only to factor out optimizations, like making this multithread
    auto child_shared = child.lock();
    if (child_shared) {
      UpdateObjects(child_shared);
    }
  }
}

// simple render pass (albedo only)
// TODO: expand so that we prepare the render context,
//       then visit each component with shadows, etc
//       prepared!
void RenderObjects(std::shared_ptr<critter::Object> obj) {
  // todo: generate this on the fly!
  RenderContext rc;
  obj->PrepareAttributes();
  obj->RenderMaterial(rc);
  for (auto child : obj->GetChildren()) {
    auto child_shared = child.lock();
    if (child_shared) {
      RenderObjects(child_shared);
    }
  }
}

GLFWwindow* InitializeGLFW(int win_width, int win_height, const std::string& window_name) {
  // initialize glfw window here
  if (!glfwInit()) {
    const char* err_desc;
    glfwGetError(&err_desc);
    BOOST_LOG_TRIVIAL(error) << "Could not initialize GLFW: " << err_desc;
    exit(EXIT_FAILURE);
  }

  BOOST_LOG_TRIVIAL(trace) << "GLFW initialized.";
  GLFWwindow* window = glfwCreateWindow(win_width, win_height, window_name.c_str(), NULL, NULL);
  if (!window) {
    BOOST_LOG_TRIVIAL(error) << "Could not create window.";
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GL_MAJOR_VERSION, 4);
  glfwWindowHint(GL_MINOR_VERSION, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    BOOST_LOG_TRIVIAL(error) << "Could not load GL calls!";
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  return window;
}

void UpdateCtx(double delta, critter::Context* ctx) {
  ctx->frame_delta_ = delta;
  ctx->FrameUpdate();
}

}
}
}