#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <critter/Model.hpp>
#include <critter/Context.hpp>

#include <shader/light/LightDataTemp.hpp>
#include <shader/materials/MatteMaterial.hpp>

#include <input/WindowEventManager.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef DEBUG
#include <shader/GLDebugSetup.hpp>
#endif

using ::monkeysworld::shader::light::LightData;
using ::monkeysworld::critter::Model;
using ::monkeysworld::critter::Context;
using ::monkeysworld::shader::materials::MatteMaterial;
using ::monkeysworld::input::WindowEventManager;

void main(int argc, char** argv) {
  // initialize GLFW
  // load model from sample OBJ
  // draw
  if (!glfwInit()) {
    BOOST_LOG_TRIVIAL(error) << "Could not initialize GLFW!";
    exit(EXIT_FAILURE);
  }

  BOOST_LOG_TRIVIAL(debug) << "GLFW initialized.";

  GLFWwindow* window = glfwCreateWindow(1600, 900, "monkey sex", NULL, NULL);
  if (!window) {
    BOOST_LOG_TRIVIAL(error) << "Failed to create window.";
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);

  glfwWindowHint(GL_MAJOR_VERSION, 4);
  glfwWindowHint(GL_MINOR_VERSION, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    BOOST_LOG_TRIVIAL(error) << "Failed to load GL calls.";
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  WindowEventManager event_mgr(window);

  #ifdef DEBUG
  ::monkeysworld::shader::gldebug::SetupGLDebug();
  #endif



  std::shared_ptr<Context> ctx = std::make_shared<Context>();
  
  // TODO: Make these pointers consistent :)
  std::shared_ptr<Model> test_model = Model::FromObjFile(ctx.get(), "resources/test/untitled4.obj");
  test_model->PrepareAttributes();

  std::vector<LightData> lights;
  lights.push_back({
    {-4.0f, 4.0f, -4.0f, 1.0f},
      1.0f,
    { 0.9f,  0.9f,  0.9f, 1.0f},
    { 0.1f,  0.1f,  0.1f, 1.0f}
  });

  MatteMaterial test_material(ctx);
  float rot = 0.0f;
  float key_y = 0.0f;
  float key_x = 0.0f;
  int x_mod = 0;
  int y_mod = 0;

  auto key_func = [&x_mod, &y_mod](int keycode, int action, int mods) {
    switch (keycode) {
      case GLFW_KEY_W:
        if (action == GLFW_PRESS) {
          x_mod = 1;
        } else if (action == GLFW_RELEASE) {
          x_mod = 0;
        }
        break;
      case GLFW_KEY_S:
        if (action == GLFW_PRESS) {
          x_mod = -1;
        } else if (action == GLFW_RELEASE) {
          x_mod = 0;
        }
        break;
      case GLFW_KEY_A:
        if (action == GLFW_PRESS) {
          y_mod = -1;
        } else if (action == GLFW_RELEASE) {
          y_mod = 0;
        }
        break;
      case GLFW_KEY_D:
        if (action == GLFW_PRESS) {
          y_mod = 1;
        } else if (action == GLFW_RELEASE) {
          y_mod = 0;
        }
        break;
    }
  };

  event_mgr.RegisterKeyListener(GLFW_KEY_W, key_func);
  event_mgr.RegisterKeyListener(GLFW_KEY_S, key_func);
  event_mgr.RegisterKeyListener(GLFW_KEY_A, key_func);
  event_mgr.RegisterKeyListener(GLFW_KEY_D, key_func);


  glfwSwapInterval(1);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  while (!glfwWindowShouldClose(window)) {
    rot += 0.01f;
    key_x += (0.01f * x_mod);
    key_y += (0.01f * y_mod);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 persp = glm::perspective(0.78f, 1.85f, 0.01f, 100.0f);
    glm::mat4 vp_matrix = glm::lookAt(glm::vec3(0, 0, -8), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    test_material.SetSurfaceColor(glm::vec4(1.0, 0.6, glm::fract(rot), 1.0));
    vp_matrix = persp * vp_matrix;
    test_model->SetPosition(glm::vec3(0, 0, 0));
    test_model->SetRotation(glm::vec3(key_x, key_y, 0));
    test_model->SetScale(glm::vec3(1.4));
    test_material.SetModelTransforms(test_model->GetTransformationMatrix());
    test_material.SetCameraTransforms(vp_matrix);
    test_material.SetLights(lights);
    test_material.UseMaterial();
    
    test_model->RenderMaterial();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}