#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <boost/log/trivial.hpp>

#include <model/Mesh.hpp>
#include <model/VertexDataContextGL.hpp>

#include <shader/light/LightDataTemp.hpp>
#include <shader/materials/MatteMaterial.hpp>
#include <shader/ShaderProgramBuilder.hpp>
#include <shader/ShaderProgram.hpp>

#include <storage/VertexPacketTypes.hpp>

#include <boost/math/constants/constants.hpp>

#include <memory>
#include <vector>

// 0 -> -1, 1 -> 1. For building cube.
#define SPLIT(x) (x * 2) - 1

using ::screenspacemanager::model::Mesh;
using ::screenspacemanager::model::VertexDataContextGL;
using ::screenspacemanager::shader::light::LightData;
using ::screenspacemanager::shader::materials::MatteMaterial;
using ::screenspacemanager::shader::ShaderProgram;
using ::screenspacemanager::shader::ShaderProgramBuilder;

using ::screenspacemanager::storage::VertexPacket3D;

static float deg_to_rad(float f) {
  return f * boost::math::constants::pi<float>() / 180.0f;
}

void main(int argc, char** argv) {

  // create mesh (probably a simple cube)
  // create MatteMaterial
  // pass static uniforms

  // bump into draw loop which will:
    // adjust transforms (i'm super shaky on where precisely the camera should go)
    // render to main fb

  if (!glfwInit()) {
    BOOST_LOG_TRIVIAL(error) << "Could not initialize GLFW!";
    exit(EXIT_FAILURE);
  }

  BOOST_LOG_TRIVIAL(debug) << "GLFW initialized.";

  GLFWwindow* test_window = glfwCreateWindow(800, 800, "when im smoking crack", NULL, NULL);
  if (!test_window) { 
    BOOST_LOG_TRIVIAL(error) << "Window creation failed!";
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GL_MAJOR_VERSION, 4);
  glfwWindowHint(GL_MINOR_VERSION, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwMakeContextCurrent(test_window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    BOOST_LOG_TRIVIAL(error) << "Could not load GL calls!";
    glfwDestroyWindow(test_window);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  Mesh<VertexPacket3D> cube_mesh(std::make_unique<VertexDataContextGL<VertexPacket3D>>());

  /**
   *  Creates the cube. 
   */ 
  for (int i = 0; i < 24; i++) {
    cube_mesh.AddVertex({
      {SPLIT((i&1)), SPLIT(((i&2)>>1)), SPLIT(((i&4)>>2))},                                             // positions
      {0, 0},                                                                                     // texcoords
      {(i > 15 ? SPLIT((i & 1)) : 0), (i > 7 && i < 16 ? SPLIT((i&2)) : 0), (i < 8 ? SPLIT((i & 4)) : 0)}   // normals
    });
  }

  cube_mesh.AddPolygon(0, 1, 2);
  cube_mesh.AddPolygon(1, 3, 2);
  cube_mesh.AddPolygon(4, 5, 6);
  cube_mesh.AddPolygon(5, 7, 6);
  cube_mesh.AddPolygon(8, 12, 9);
  cube_mesh.AddPolygon(9, 12, 13);
  cube_mesh.AddPolygon(10, 14, 11);
  cube_mesh.AddPolygon(11, 14, 15);
  cube_mesh.AddPolygon(16, 20, 18);
  cube_mesh.AddPolygon(20, 22, 18);
  cube_mesh.AddPolygon(17, 19, 21);
  cube_mesh.AddPolygon(21, 19, 23);

  std::vector<LightData> lights;
  // creates a new white light at -1, -1, -1
  lights.push_back({
    {-1.0f, 1.0f, 0.0f, 1.0f},
      1.0f,
    { 0.9f,  0.9f,  0.9f, 1.0f},
    { 0.1f,  0.1f,  0.1f, 1.0f}
  });

  MatteMaterial test_material;
  
  test_material.SetSurfaceColor(glm::vec4(1.0, 0.6, 0.0, 1.0));

  float rot = 0.0f;
  while (!glfwWindowShouldClose(test_window)) { 
    rot += 0.01f;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glViewport(0, 0, 800, 800);
    glm::mat4 model_matrix = glm::mat4(1.0);
    glm::mat4 vp_matrix = glm::mat4(1.0);
    // glm::perspective now takes a radians arg
    glm::mat4 persp = glm::perspective(deg_to_rad(45.0f), 1.0f, 0.01f, 100.0f);
    vp_matrix = persp * vp_matrix;
    test_material.SetSurfaceColor(glm::vec4(1.0, 0.6, glm::fract(rot), 1.0)) ;

    model_matrix = glm::translate(model_matrix, glm::vec3(0, 0, -4));
    model_matrix = glm::rotate(model_matrix, rot, glm::vec3(0.707));
    test_material.SetModelTransforms(model_matrix);
    test_material.SetCameraTransforms(vp_matrix);
    test_material.SetLights(lights);
    test_material.UseMaterial();

    cube_mesh.PointToVertexAttribs();
    glPointSize(16.0f); 
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
    glfwSwapBuffers(test_window);
    glfwSwapInterval(1);
    glfwPollEvents();
  }

  glfwDestroyWindow(test_window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}