#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <engine/BaseEngine.hpp>

// TODO: create an actual logging setup -- we can config it in init :)
#include <boost/log/trivial.hpp>


namespace monkeysworld {
namespace engine {
namespace baseengine {

void GameLoop(std::shared_ptr<Scene> scene, std::shared_ptr<critter::Context> ctx, GLFWwindow* window) {
  // initialize the scene
  for (;;) {
    // check to see if a new scene needs to be initialized
    // if so: do that

    // poll for events
    // process all incoming inputs
    // use a friend func to update some constants inherent to the context
    //   ex. the frame delta
    // visit objects in our component tree and call their "update" funcs
    // visit objects again with a "renderer"
    // swap buffers
    // loop back
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

}
}
}