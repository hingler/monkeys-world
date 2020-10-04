#include <shader/ShaderProgramBuilder.hpp>
#include <shader/InvalidShaderException.hpp>
#include <shader/LinkFailedException.hpp>
#include <glad/glad.h>

#include <fstream>

namespace screenspacemanager {
namespace shader {

using std::ios_base;

ShaderProgramBuilder::ShaderProgramBuilder() {
  shaders_ = ShaderPacket();
}

ShaderProgramBuilder& ShaderProgramBuilder::WithVertexShader(const std::string& vertex_path) {
  shaders_.vertex_shader = CreateShaderFromFile(vertex_path, GL_VERTEX_SHADER);
  return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::WithGeometryShader(const std::string& geometry_path) {
  shaders_.geometry_shader = CreateShaderFromFile(geometry_path, GL_GEOMETRY_SHADER);
  return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::WithFragmentShader(const std::string& fragment_path) {
  shaders_.fragment_shader = CreateShaderFromFile(fragment_path, GL_FRAGMENT_SHADER);
  return *this;
}

ShaderProgram ShaderProgramBuilder::Build() {
  GLuint prog = glCreateProgram();

  AttachIfNonZero(prog, shaders_.vertex_shader);
  AttachIfNonZero(prog, shaders_.fragment_shader);
  AttachIfNonZero(prog, shaders_.geometry_shader);
  glLinkProgram(prog);

  GLint success;
  glGetProgramiv(prog, GL_LINK_STATUS, &success);
  if (!success) {
    std::string error_msg;
    error_msg.reserve(512);
    glGetProgramInfoLog(prog, 512, NULL, &error_msg[0]); 

    throw LinkFailedException("Link failed: " + error_msg);
  }

  return ShaderProgram(prog);
}

ShaderProgramBuilder::~ShaderProgramBuilder() {
  DeleteIfNonZero(shaders_.vertex_shader);
  DeleteIfNonZero(shaders_.fragment_shader);
  DeleteIfNonZero(shaders_.geometry_shader);
}

void ShaderProgramBuilder::AttachIfNonZero(GLuint prog, GLuint shader) {
  // docs: valid shader is non 0
  if (shader != 0) {
    glAttachShader(prog, shader);
  }
}

void ShaderProgramBuilder::DeleteIfNonZero(GLuint shader) {
  if (shader != 0) {
    glDeleteShader(shader);
  }
}

// todo: separate file reading into a separate class? it's a bit trivial though :/
GLuint ShaderProgramBuilder::CreateShaderFromFile(const std::string& shader_path, GLenum shader_type) {
  GLuint shader = glCreateShader(shader_type);
  std::ifstream shader_file(shader_path);
  
  if (shader_file.fail()) {
    // could not read file path
    shader_file.close();
    glDeleteShader(shader);
    throw InvalidShaderException("Invalid shader path " + shader_path);
  }

  std::string contents;
  shader_file.seekg(0, ios_base::end);
  std::streamoff file_size = shader_file.tellg();
  shader_file.seekg(0, ios_base::beg);
  contents.resize(file_size);
  shader_file.read(&contents[0], file_size);

  shader_file.close();

  const char* shader_data = contents.c_str();
  glShaderSource(shader, 1, &shader_data, NULL);

  glCompileShader(shader);
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (!success) {
    std::string error_msg;

    // attempt to allocate sufficient space
    error_msg.reserve(512);
    glGetShaderInfoLog(shader, 512, NULL, &error_msg[0]);
    throw InvalidShaderException("Shader failed to compile: " + error_msg);
  }

  return shader;
}

} // namespace shader
} // namespace screenspacemanager