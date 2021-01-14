#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <shader/light/LightDataTemp.hpp>
#include <shader/light/LightTypes.hpp>

#include <glm/glm.hpp>

#include <glad/glad.h>

#include <vector>

namespace monkeysworld {
namespace shader {

/**
 *  Represents a material which is applied to an in-engine object.
 */ 
class Material {
 public:
  /**
   *  Prepares openGL to draw with this material by passing all uniforms.
   */ 
  virtual void UseMaterial() = 0;

  virtual GLuint GetProgramDescriptor() = 0;
};

} // namespace shader
} // namespace monkeysworld

#endif