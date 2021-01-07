// simple test program which displays our glyph atlas
// ensures that glyphs are being rendered correctly

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <font/Font.hpp>

#include <shader/GLDebugSetup.hpp>

#include <iostream>

const char* shader_vert = 
"#version 430 core\n                                      \
\n                                                        \
\n                                                        \
layout(location = 0) in vec4 position;\n                  \
layout(location = 1) in vec4 texcoord;\n                  \
\n                                                        \
layout(location = 0) out vec4 position_output;\n          \
layout(location = 1) out vec4 texcoord_output;\n          \
\n                                                        \
void main() {\n                                           \
  position_output = position;\n                           \
  texcoord_output = texcoord;\n                           \
  gl_Position = position;\n                               \
}\n                                                       \
";

const char* shader_frag = 
"#version 430 core\n                                      \
\n                                                        \
\n                                                        \
layout(location = 0) in vec4 position;\n                  \
layout(location = 1) in vec4 texcoord;\n                  \
\n                                                        \
layout(location = 0) uniform sampler2D glyph_texture;\n   \
layout(location = 1) uniform float time;\n                \
\n                                                        \
layout(location = 0) out vec4 fragColor;\n                \
\n                                                        \
void main() {\n                                                             \
  float texval = texture(glyph_texture, texcoord.xy).r;\n\
  fragColor = vec4(texval, 0.0, 0.0, texval);\n       \
}\n                                                                         \
";

using ::monkeysworld::font::Font;
using namespace ::monkeysworld::shader::gldebug;
using namespace ::monkeysworld;

void errorfun(int a, const char* b) {
  std::cout << "error are occur" << std::endl;
  std::cout << b << std::endl;
}

int main(int argc, char** argv) {
  glfwSetErrorCallback(errorfun);
  if (!glfwInit()) {
    // failed to run
    std::cout << "init crash" << std::endl;
    exit(EXIT_FAILURE);
  }

  GLFWwindow* win = glfwCreateWindow(1024, 768, "hello spongebob", NULL, NULL);
  if (!win) {
    std::cout << "window create failed" << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwMakeContextCurrent(win);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    glfwDestroyWindow(win);
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  SetupGLDebug();

  // create some shitty square geom
  float geom_data[12] = {-1.0, -1.0,
                         -1.0,  1.0,
                          1.0,  1.0,
                          1.0,  1.0,
                          1.0, -1.0,
                         -1.0, -1.0};
  
  GLuint buf;
  glGenBuffers(1, &buf);
  glBindBuffer(GL_ARRAY_BUFFER, buf);
  glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), geom_data, GL_STATIC_DRAW);
  
  GLuint prog = glCreateProgram();
  GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_id, 1, &shader_vert, NULL);
  glCompileShader(vert_id);

  GLint success;
  glGetShaderiv(vert_id, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    std::cout << "fucked up the vertex shader" << std::endl;
    std::string error_msg;
    error_msg.resize(512);
    glGetShaderInfoLog(vert_id, 512, NULL, &error_msg[0]); 
    std::cout << "Shader failed: " << error_msg << std::endl;
    exit(EXIT_FAILURE);
  }

  GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_id, 1, &shader_frag, NULL);
  glCompileShader(frag_id);
  glGetShaderiv(frag_id, GL_COMPILE_STATUS, &success);
  if (success != GL_TRUE) {
    std::cout << "fucked up the fragment shader" << std::endl;
    exit(EXIT_FAILURE);
  }

  glAttachShader(prog, vert_id);
  glAttachShader(prog, frag_id);
  glLinkProgram(prog);
  glGetProgramiv(prog, GL_LINK_STATUS, &success);
  if (success != GL_TRUE) {
    std::cout << "link failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "ok" << std::endl;
  Font f("resources/Montserrat-Light.ttf");
  std::cout << "ok" << std::endl;

  auto the = f.GetTextGeometry("hello spongebob", 120);

  the->PointToVertexAttribs();

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, f.GetGlyphAtlas());
  glProgramUniform1i(prog, 0, 0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

  // the shit
  glUseProgram(prog);
  glfwSwapInterval(1);
  float time = 0.0;
  glClearColor(0.1, 0.1, 0.1, 1.0);
  int framecount = 0;
  glViewport(0, 0, 1024, 1024);
  while (!glfwWindowShouldClose(win)) {
    std::cout << framecount << std::endl;
    time += 0.01;
    std::string test = std::to_string(framecount++);
    the = f.GetTextGeometry(test, 96); 
    glProgramUniform1f(prog, 1.0f, time);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    the->PointToVertexAttribs();
    glUseProgram(prog);
    // streaming on discord for some reason unbinds this active texture
    // be sure to bind it when rendering in the real world! :)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, f.GetGlyphAtlas());
    glUniform1i(0, 0);
    glDrawElements(GL_TRIANGLES, the->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
    glfwSwapBuffers(win);
    glfwPollEvents();
  }

  return 0;
}