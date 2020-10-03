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
   */ 
  ShaderProgram(GLuint prog);

  /**
   *  Returns the descriptor associated with the constructed shader program.
   */ 
  GLuint GetProgramDescriptor();

  // dtor
  ~ShaderProgram();
  // cctor
  ShaderProgram(const ShaderProgram& other) = delete;
  // move ctor
  ShaderProgram(ShaderProgram&& other);
  // assignment copy
  ShaderProgram& operator=(const ShaderProgram& other) = delete;
  // assignment move
  ShaderProgram& operator=(ShaderProgram&& other);

 private:

  // this is better suited for an assert statement, probably

  // descriptor for program
  GLuint prog_;
};

};    // namespace shader
};    // namespace screenspacemanager


#endif  // SHADER_PROGRAM_H_