#include <shader/ShaderProgramBuilder.hpp>
#include <shader/InvalidShaderException.hpp>
#include <shader/LinkFailedException.hpp>
#include <glad/glad.h>

#include <fstream>

namespace screenspacemanager {
namespace shader {

using std::ios_base;

ShaderProgramBuilder::ShaderProgramBuilder() {
  prog_ = glCreateProgram();
}

ShaderProgramBuilder& ShaderProgramBuilder::WithVertexShader(const std::string& vertex_path) {
  CreateShaderFromFile(vertex_path, GL_VERTEX_SHADER);
  return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::WithGeometryShader(const std::string& geometry_path) {
  CreateShaderFromFile(geometry_path, GL_GEOMETRY_SHADER);
  return *this;
}

ShaderProgramBuilder& ShaderProgramBuilder::WithFragmentShader(const std::string& fragment_path) {
  CreateShaderFromFile(fragment_path, GL_FRAGMENT_SHADER);
  return *this;
}

ShaderProgram ShaderProgramBuilder::Build() {
  glLinkProgram(prog_);
  GLint success;
  glGetProgramiv(prog_, GL_LINK_STATUS, &success);
  if (!success) {
    std::string error_msg;
    error_msg.reserve(512);
    glGetProgramInfoLog(prog_, 512, NULL, &error_msg[0]); 
    // contiguous memory guaranteed in c++11 and later
    throw LinkFailedException(error_msg);
  }
}

void ShaderProgramBuilder::CreateShaderFromFile(const std::string& shader_path, GLenum shader_type) {
  GLuint shader = glCreateShader(shader_type);
  std::ifstream shader_file(shader_path);

  if (shader_file.fail()) {
    // could not read file path
    shader_file.close();
    glDeleteShader(shader);
    throw std::invalid_argument("Invalid shader path.");
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
    throw InvalidShaderException(error_msg);
  }

  glAttachShader(prog_, shader);
}

} // namespace shader
} // namespace screenspacemanager