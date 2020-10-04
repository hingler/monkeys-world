#ifndef SHADER_PROGRAM_H_
#define SHADER_PROGRAM_H_

#include <string>
#include <glad/glad.h>

namespace screenspacemanager {
namespace shader {

/**
 *  RAII wrapper for shaders.
 *  Also provides utilities for constructing shaders from file.
 */ 
class ShaderProgram {
 public:
 
  /**
   *  Constructs a new shader program from a GL program descriptor.
   *  @param prog - Shader passed in. This class assumes ownership.
   */ 
  ShaderProgram(GLuint prog);

  /**
   *  Returns the descriptor associated with the constructed shader program.
   */ 
  GLuint GetProgramDescriptor();

  ~ShaderProgram();
  ShaderProgram(const ShaderProgram& other) = delete;
  ShaderProgram(ShaderProgram&& other);
  ShaderProgram& operator=(const ShaderProgram& other) = delete;
  ShaderProgram& operator=(ShaderProgram&& other);

 private:
  // descriptor for program
  GLuint prog_;
};

};    // namespace shader
};    // namespace screenspacemanager


#endif  // SHADER_PROGRAM_H_