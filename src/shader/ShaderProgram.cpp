#include <shader/ShaderProgram.hpp>
#include <glad/glad.h>
namespace screenspacemanager {
namespace shader {

ShaderProgram::ShaderProgram(GLuint prog) {
  prog_ = prog;
}

GLuint ShaderProgram::GetProgramDescriptor() {
  return prog_;
}

ShaderProgram::~ShaderProgram() {
  glDeleteProgram(prog_);
}

ShaderProgram::ShaderProgram(ShaderProgram&& other) {
  prog_ = other.prog_;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) {
  prog_ = other.prog_;
  return *this;
}

} // namespace shader
} // namespace screenspacemanager