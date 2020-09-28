#include "layer/Layer.hpp"
#include "Paint.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace screenspacemanager {
namespace layer {
Layer::Layer() : Layer(128, 128) { }

Layer::Layer(int width, int height) {
  // create framebuffer
  // thats it really
  glGenFramebuffers(1, &framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

  dims_ = {width, height};

  glGenTextures(2, &framebuffer_color_);    // should be contiguous
  GenerateFramebufferTextures();

  id_ = id_generator_.CreateUniqueID();

  // generate some random ID which can be used here
  // 

}

void Layer::SetDimensions(Point dims) {
  SetDimensions(dims.x, dims.y);
}

void Layer::SetDimensions(int width, int height) {
  dims_.x = width;
  dims_.y = height;
}

void Layer::SetPosition(Point coords) {
  SetPosition(coords.x, coords.y);
}

void Layer::SetPosition(int x, int y) {
  coords_.x = x;
  coords_.y = y;
}

const Point* Layer::GetDimensions() {
  return &dims_;
}

const Point* Layer::GetPosition() {
  return &coords_;
}

void Layer::BindFramebufferAndRender() {
  // todo: handle invalidated views
  glViewport(0, 0, dims_.x, dims_.y);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
}

uint64_t Layer::GetLayerId() {
  return id_;
}

void Layer::SetLayerId(int id) {
  id_generator_.InsertUniqueID(id);
  id_ = id;
}

GLuint Layer::GetFramebufferColor() {

  return framebuffer_color_;
}

// layer count should stay small so a vector is probably fine?
// query each sub layer for the ID and see if it's there with a simple equality check
// if a layer's ID chances we lose if in the hashing method
// groups will check their children
// leaves will return themselves if the passed ID matches theirs.

void Layer::GenerateFramebufferTextures() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, framebuffer_color_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dims_.x, dims_.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_color_, 0);

  glBindTexture(GL_TEXTURE_2D, framebuffer_depth_stencil_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, dims_.x, dims_.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebuffer_depth_stencil_, 0);
}

bool Layer::CompileProgram(std::string vertex_path, std::string fragment_path, GLuint* output) {
  GLuint vert_shader;
  GLuint frag_shader;
  bool success = CompileShader(vertex_path, GL_VERTEX_SHADER, &vert_shader);
  success &= CompileShader(fragment_path, GL_FRAGMENT_SHADER, &frag_shader);

  if (!success) {
    return false;
  }

  GLuint prog = glCreateProgram();
  glAttachShader(prog, vert_shader);
  glAttachShader(prog, frag_shader);
  glLinkProgram(prog);

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  GLint link_status;
  glGetProgramiv(prog, GL_LINK_STATUS, &link_status);
  if (!link_status) {
    // failed to link shaders!
    char info[512];
    glGetProgramInfoLog(prog, 512, NULL, info);
    std::cout << "Failed to link shader\n" << info << std::endl;
    return false;
  }

  return true;
}

bool Layer::CompileShader(std::string shader_path, GLuint type, GLuint* output) {
  std::ifstream shader_file(shader_path);
  if (shader_file.bad() || shader_file.fail()) {
    shader_file.close();
    return false;
  }

  std::stringstream shader_stream;
  shader_stream << shader_file.rdbuf();

  *output = glCreateShader(type);
  const char* shader_data = shader_stream.str().c_str();
  glShaderSource(*output, 1, &shader_data, NULL);

  GLint success;
  glCompileShader(*output);

  shader_file.close();
  glGetShaderiv(*output, GL_COMPILE_STATUS, &success);
  if (!success) {
    // failed to compile
    // debug only
    // replace with logging soln
    char info[512];
    glGetShaderInfoLog(*output, 511, NULL, info);
    std::cout << "error while compiling shader\n" << info << std::endl;
    glDeleteShader(*output);
    return false;
  }

  return true;
};
};  // namespace layer
};  // namespace screenspacemanager