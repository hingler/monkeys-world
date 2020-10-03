#ifndef SHADER_PROGRAM_BUILDER_H_
#define SHADER_PROGRAM_BUILDER_H_

#include <string>
#include <glad/glad.h>

#include <shader/ShaderProgram.hpp>

namespace screenspacemanager {
namespace shader {

/**
 *  Class for building shader programs from individual shaders.
 */ 
class ShaderProgramBuilder {

 public:

  /**
   *  Creates a new shader program builder.
   */ 
  ShaderProgramBuilder();

  ShaderProgramBuilder& WithVertexShader(const std::string& vertex_path);
  ShaderProgramBuilder& WithGeometryShader(const std::string& geometry_path);
  ShaderProgramBuilder& WithFragmentShader(const std::string& fragment_path);
  ShaderProgram Build();

 private:
  /**
   *  Compiles and verifies proper functionality, before attaching it to the program.
   */ 
  void CreateShaderFromFile(const std::string& shader_path, GLenum shader_type);

  // contains the program being created
  GLuint prog_;

};

}   // namespace shader  
}   // namespace screenspacemanager

#endif  // SHADER_PROGRAM_FACTORY_H_