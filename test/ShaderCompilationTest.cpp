// need to spin up the GL machine for this one :)

#include <shader/ShaderProgramBuilder.hpp>
#include <shader/ShaderProgram.hpp>

#include <gtest/gtest.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


namespace screenspacemanagertest {

using screenspacemanager::shader::ShaderProgramBuilder;
using screenspacemanager::shader::ShaderProgram;

class ShaderBuilderTests : public ::testing::Test {
 protected:
  void SetUp() override {
    if (!glfwInit()) {
      // could not initialize glfw, required for shader funcs
      exit(EXIT_FAILURE);
    }

    test_window = glfwCreateWindow(800, 600, "temp", NULL, NULL);
    if (test_window == NULL) {
      // could not create window
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    // currently using opengl 4.1
    glfwWindowHint(GL_MAJOR_VERSION, 4);
    glfwWindowHint(GL_MINOR_VERSION, 1);

    // rig this test up so that it only runs when on an env where it can gen a window

    glfwMakeContextCurrent(test_window);

    // context (glfwwindow has one) is required initially
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      // could not load GL calls
      glfwDestroyWindow(test_window);
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

    // normally some other setup is required -- here, however, we're just making shader compile calls
  }

  void TearDown() override {
    glfwDestroyWindow(test_window);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

 private:
  GLFWwindow* test_window;
};

TEST_F(ShaderBuilderTests, CreateEmptyShader) {
  ShaderProgram prog = ShaderProgramBuilder()
                        .WithVertexShader("resources/test/dummy-shader.vert")
                        .WithFragmentShader("resources/test/dummy-shader.frag")
                        .Build();
  
}

}  // namespace screenspacemanagertest