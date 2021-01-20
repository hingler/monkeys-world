#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine/BaseEngine.hpp>
#include <engine/RenderContext.hpp>

#include <critter/visitor/ActiveCameraFindVisitor.hpp>
#include <critter/visitor/LightVisitor.hpp>

#include <shader/light/LightTypes.hpp>

// TODO: create an actual logging setup -- we can config it in init :)
#include <boost/log/trivial.hpp>

#include <critter/ui/UIObject.hpp>

#ifdef DEBUG
#include <shader/GLDebugSetup.hpp>
#endif

#include <chrono>


namespace monkeysworld {
namespace engine {
namespace baseengine {

using ::monkeysworld::critter::Camera;
using ::monkeysworld::critter::visitor::ActiveCameraFindVisitor;
using ::monkeysworld::critter::visitor::LightVisitor;

using ::monkeysworld::shader::Material;
using ::monkeysworld::shader::light::SpotLight;
using ::monkeysworld::shader::light::spotlight_info;

using ::monkeysworld::critter::ui::UIObject;

using ::monkeysworld::engine::RenderContext;

/**
 *  Calls update funcs on all objects.
 */ 
static void UpdateObjects(std::shared_ptr<critter::Object>);

/**
 *  Renders all objects nested within the passed root.
 */ 
static void RenderObjects(std::shared_ptr<critter::Object>, RenderContext&);

/**
 *  Renders all UI objects.
 */ 
static void RenderUI(std::shared_ptr<critter::Object>, RenderContext&);

// subtype context to enable access to frequent update functions
// pass supertype to scene
void GameLoop(std::shared_ptr<engine::Context> ctx, GLFWwindow* window) {
  #ifdef DEBUG
  if (GLAD_GL_ARB_debug_output) {
    BOOST_LOG_TRIVIAL(debug) << "GL debug output supported!";
    // TODO: switch debug setup over to extension jargon
    ::monkeysworld::shader::gldebug::SetupGLDebug();
  }
  #endif

  
  // setup timing
  auto start = std::chrono::high_resolution_clock::now();
  auto finish = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::ratio<1, 1>> dur = finish - start;

  LightVisitor light_visitor;
  ActiveCameraFindVisitor cam_visitor;
  

  RenderContext rc;
  std::vector<spotlight_info> spotlights;

  glfwSwapInterval(1);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  while(!glfwWindowShouldClose(window)) {
    start = std::chrono::high_resolution_clock::now();
    // reset any visitors which store info
    light_visitor.Clear();
    cam_visitor.Clear();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    // poll for events
    // check to see if a new scene needs to be initialized
    // if so: do that

    UpdateCtx(dur.count(), ctx.get());

    // process all incoming inputs
    // use a friend func to update some constants inherent to the context
    //   ex. the frame delta
    // visit objects in our component tree and call their "update" funcs
    auto scene = ctx->GetScene();
    UpdateObjects(scene->GetGameObjectRoot());
    UpdateObjects(scene->GetUIObjectRoot());
    scene->GetGameObjectRoot()->Accept(light_visitor);
    scene->GetGameObjectRoot()->Accept(cam_visitor);
    // for each light:
    //   - do a depth render from the perspective of our lights


    // for now: put the lights by themselves in our render context

    // SHADOW PASS -- get shadow info
    // add light info to the render context, in appropriate places
    // shadow pass is still a TODO. :)

    // only spotlights for now -- the goal:
    //   - see which lights are in the scene
    //   - run the light pass for each one
    //   - note: there should be a method which contains all GL calls required to draw the object
    //   - that way, we can separate material configuration from drawing
    //   - add a method to Object which accomplishes this

    // MEMORY ISSUE: spotlights were never cleared, so the list just builds and builds -- slowdown increases over time because we spend so much time
    // passing around a massive array of spotlights
    spotlights.clear();
    for (auto light : light_visitor.GetSpotLights()) {
      // no render context needed -- we're just preparing attributes and calling a default shadow func
      // we want to have the shadow map shader prepared beforehand

      // create a simple visitor which will pass this light's properties to the program and render to the map
      // lastly: add the light to the render context
      spotlights.push_back(light->GetSpotLightInfo());
    }
    // lights should be able to generate some packet which the renderer can use
    // render objects, using the render context
    // swap buffers
    rc.SetSpotlights(spotlights);
    // TODO: handle null case with a default camera :(
    rc.SetActiveCamera(std::static_pointer_cast<Camera>(cam_visitor.GetActiveCamera()));
    // then: just draw (for now)
    //    if needed, we can start to collect additional information and add it to the context
    //    however -- for now, just this.
    glBindFramebuffer(GL_FRAMEBUFFER, NULL);
    int w, h;
    ctx->GetFramebufferSize(&w, &h);
    glViewport(0, 0, w, h);
    RenderObjects(scene->GetGameObjectRoot(), rc);
    
    // render UI
    if (scene->GetUIObjectRoot()) {
      RenderUI(scene->GetUIObjectRoot(), rc);
      glViewport(0, 0, w, h);
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      auto ui_root = std::dynamic_pointer_cast<UIObject>(scene->GetUIObjectRoot());
      if (ui_root) {
        ui_root->DrawToScreen();
      }
    }
    // render UI on top
    // call drawtoscreen on ui root
    // create render context
    // add lights to the render context
    // then: render!
    glfwSwapBuffers(window);
    // loop back
    glfwPollEvents();
    finish = std::chrono::high_resolution_clock::now();
    dur = finish - start;
  }
}

void UpdateObjects(std::shared_ptr<critter::Object> obj) {
  // call update func
  obj->Update();
  for (auto child : obj->GetChildren()) {
    // if child exists, visit it and queue up its children
    // TODO: write a visitor, only to factor out optimizations, like making this multithread
    UpdateObjects(child);
  }
}

// simple render pass (albedo only)
// TODO: expand so that we prepare the render context,
//       then visit each component with shadows, etc
//       prepared!
void RenderObjects(std::shared_ptr<critter::Object> obj, RenderContext& rc) {
  // todo: generate this on the fly!
  if (!obj) {
    // nothing to render!
    return;
  }

  obj->PrepareAttributes();
  obj->RenderMaterial(rc);
  for (auto child : obj->GetChildren()) {
    RenderObjects(child, rc);
  }
}

void RenderUI(std::shared_ptr<critter::Object> obj, RenderContext& rc) {
  if (!obj) {
    return;
  }

  // note: components can fuck it up when they want to :)
  obj->RenderMaterial(rc);
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

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

void UpdateCtx(double delta, engine::Context* ctx) {
  // don't like this at all
  ctx->frame_delta_ = delta;
  ctx->FrameUpdate();
}

}
}
}