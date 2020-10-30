#include <shader/ShaderProgram.hpp>
#include <shader/exception/UninitializedShaderException.hpp>

#include <glad/glad.h>

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace shader {

using exception::UninitializedShaderException;

ShaderProgram::ShaderProgram() {
  prog_ = 0;
}

ShaderProgram::ShaderProgram(GLuint prog) {
  prog_ = prog;
}

GLuint ShaderProgram::GetProgramDescriptor() {

  if (!prog_) {
    BOOST_LOG_TRIVIAL(error) << "ShaderProgram was never initialized!";
    throw UninitializedShaderException("Shader was never initialized");
  }

  return prog_;
}

ShaderProgram::~ShaderProgram() {
  if (prog_) {
    glDeleteProgram(prog_);
  }
  
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) {
  prog_ = other.prog_;
  other.prog_ = 0;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) {
  prog_ = other.prog_;
  other.prog_ = 0;
  // other was being dtor'd and thus the program disappeared
  return *this;
}

} // namespace shader
} // namespace monkeysworld