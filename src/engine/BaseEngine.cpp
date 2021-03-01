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
 *  Calls create funcs on all objects in the hierarchy.
 */ 
static void CreateObjects(std::shared_ptr<critter::Object>);

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
void GameLoop(std::shared_ptr<engine::EngineContext> ctx, GLFWwindow* window) {
  #ifdef DEBUG
  if (GLAD_GL_ARB_debug_output) {
    BOOST_LOG_TRIVIAL(debug) << "GL debug output supported!";
    ::monkeysworld::shader::gldebug::SetupGLDebug();
  }
  #endif

  LightVisitor light_visitor;
  ActiveCameraFindVisitor cam_visitor;
  

  RenderContext rc;
  std::vector<spotlight_info> spotlights;

  glfwSwapInterval(0);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  ctx->InitializeScene();

  CreateObjects(ctx->GetScene()->GetGameObjectRoot());
  CreateObjects(std::dynamic_pointer_cast<EngineWindow>(ctx->GetScene()->GetWindow())->GetRootObject());

  while(!glfwWindowShouldClose(window)) {
    // reset any visitors which store info
    light_visitor.Clear();
    cam_visitor.Clear();

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    auto win = std::dynamic_pointer_cast<EngineWindow>(ctx->GetScene()->GetWindow());

    // swap to the new context if it's ready :)
    if (auto ctx_new = ctx->GetNewContext()) {
      ctx = ctx_new;
      auto scene = ctx_new->GetScene();
      CreateObjects(scene->GetGameObjectRoot());
      win = std::dynamic_pointer_cast<EngineWindow>(scene->GetWindow());
      CreateObjects(win->GetRootObject());
    }

    auto scene = ctx->GetScene();
    if (scene->GetGameObjectRoot()) {
      UpdateObjects(scene->GetGameObjectRoot());
      scene->GetGameObjectRoot()->Accept(light_visitor);
      scene->GetGameObjectRoot()->Accept(cam_visitor);
    }

    UpdateObjects(win->GetRootObject());
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

      spotlights.push_back(light->GetSpotLightInfo());
    }
    rc.SetSpotlights(spotlights);
    rc.SetActiveCamera(std::static_pointer_cast<Camera>(cam_visitor.GetActiveCamera()));
    ctx->GetCurrentFrame()->BindFramebuffer(shader::FramebufferTarget::DEFAULT);
    int w, h;
    ctx->GetFramebufferSize(&w, &h);
    glViewport(0, 0, w, h);
    RenderObjects(scene->GetGameObjectRoot(), rc);

    
    
    // render UI
    win->Layout();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    RenderUI(win->GetRootObject(), rc);
    glViewport(0, 0, w, h);
    ctx->GetCurrentFrame()->BindFramebuffer(shader::FramebufferTarget::DEFAULT);
    auto ui_root = std::dynamic_pointer_cast<UIObject>(win->GetRootObject());
    if (ui_root) {
      ui_root->DrawToScreen();
    }

    glEnable(GL_DEPTH_TEST);

    // blit last frame to current
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    ctx->GetCurrentFrame()->BindFramebuffer(shader::FramebufferTarget::READ);
    glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glfwSwapBuffers(window);
    glfwPollEvents();
    ctx->UpdateContext();
    ctx->GetCurrentFrame()->BindFramebuffer(shader::FramebufferTarget::DEFAULT);
  }
}

void CreateObjects(std::shared_ptr<critter::Object> obj) {
  obj->Create();
  for (auto child : obj->GetChildren()) {
    CreateObjects(child);
  }
}

void UpdateObjects(std::shared_ptr<critter::Object> obj) {
  // call update func
  obj->Update();
  for (auto child : obj->GetChildren()) {
    // if child exists, visit it and queue up its children
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

}
}
}