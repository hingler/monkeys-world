#ifndef SHADER_PROGRAM_BUILDER_H_
#define SHADER_PROGRAM_BUILDER_H_

#include <string>
#include <glad/glad.h>

#include <shader/ShaderProgram.hpp>

namespace screenspacemanager {
namespace shader {

/**
 *  FDs associated with shaders
 *  (no tesselation shader because i dont think im gonna use that)
 */ 
struct ShaderPacket {
  // fd for vertex shader
  GLuint vertex_shader;

  // fd for fragment shader
  GLuint fragment_shader;

  // fd for geometry shader
  GLuint geometry_shader;
};

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

  ~ShaderProgramBuilder();

 private:
  /**
   *  Compiles and verifies proper functionality, before attaching it to the program.
   */ 
  GLuint CreateShaderFromFile(const std::string& shader_path, GLenum shader_type);

  /**
   *  Attaches the shader if its value is non-zero.
   */ 
  void AttachIfNonZero(GLuint prog, GLuint shader);

  /**
   *  Deletes non-zero shaders. Will only be called from dtor, so setting to 0
   *  should not matter
   */ 
  void DeleteIfNonZero(GLuint shader);

  // contains the program being created
  GLuint prog_;
  ShaderPacket shaders_;

};

}   // namespace shader  
}   // namespace screenspacemanager

#endif  // SHADER_PROGRAM_FACTORY_H_