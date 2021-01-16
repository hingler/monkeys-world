#include <shader/ShaderProgramBuilder.hpp>
#include <shader/exception/InvalidShaderException.hpp>
#include <shader/exception/LinkFailedException.hpp>
#include <glad/glad.h>

#include <boost/log/trivial.hpp>

#include <fstream>

namespace monkeysworld {
namespace shader {

using std::ios_base;
using exception::InvalidShaderException;
using exception::LinkFailedException;
using file::CachedFileLoader;


static std::string GetShaderType(GLint type) {
  switch (type) {
  case GL_VERTEX_SHADER:
    return "Vertex Shader";
    break;
  case GL_FRAGMENT_SHADER:
    return "Fragment Shader";
    break;
  default:
    return "Something else";
  }
}

ShaderProgramBuilder::ShaderProgramBuilder(std::shared_ptr<CachedFileLoader> loader) {
  shaders_ = ShaderPacket();
  prog_ = 0;
  loader_ = loader;
}

ShaderProgramBuilder::ShaderProgramBuilder(ShaderProgramBuilder&& other) {
  shaders_ = std::move(other.shaders_);
  memset(&other.shaders_, 0, sizeof(ShaderPacket));
  prog_ = other.prog_;
  other.prog_ = 0;
  loader_ = std::move(other.loader_);
}

ShaderProgramBuilder& ShaderProgramBuilder::operator=(ShaderProgramBuilder&& other) {
  shaders_ = std::move(other.shaders_);
  memset(&other.shaders_, 0, sizeof(ShaderPacket));
  prog_ = other.prog_;
  other.prog_ = 0;
  loader_ = other.loader_;
  return *this;
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
  if (success != GL_TRUE) {
    std::string error_msg;
    error_msg.resize(512);
    glGetProgramInfoLog(prog, 512, NULL, &error_msg[0]); 
    BOOST_LOG_TRIVIAL(error) << "Shader link failed: " << error_msg;
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
    BOOST_LOG_TRIVIAL(debug) << "Attached shader to prog " << prog;
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
  auto buffer = loader_->LoadFile(shader_path);
  std::istream shader_file(&buffer);
  
  if (!shader_file.good()) {
    // could not read file path
    glDeleteShader(shader);
    BOOST_LOG_TRIVIAL(error) << "Invalid shader path " << shader_path;
    throw std::invalid_argument("Invalid shader path " + shader_path);
  }

  std::string contents;
  shader_file.seekg(0, ios_base::end);
  std::streamoff file_size = shader_file.tellg();
  shader_file.seekg(0, ios_base::beg);
  contents.resize(file_size);
  shader_file.read(&contents[0], file_size);

  const char* shader_data = contents.c_str();
  glShaderSource(shader, 1, &shader_data, NULL);

  glCompileShader(shader);
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (success != GL_TRUE) {
    std::string error_msg;
    GLint log_size;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_size);
    BOOST_LOG_TRIVIAL(debug) << "ERROR CODE: " << std::to_string(success);
    error_msg.resize(std::string::size_type(log_size));
    glGetShaderInfoLog(shader, log_size, NULL, &error_msg[0]);
    BOOST_LOG_TRIVIAL(error) << GetShaderType(shader_type) << " failed to compile: " << error_msg;
    throw InvalidShaderException("Shader failed to compile: " + error_msg);
  }

  return shader;
}

} // namespace shader
} // namespace monkeysworld