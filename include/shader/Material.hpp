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

  /**
   *  Passes light data to the material.
   *  (depreciated -- remove later)
   *  @param lights - a list containing all lights in the scene, in global coords.
   */ 
  virtual void SetLights(const std::vector<light::LightData>& lights) = 0;

  /**
   *  Passes spotlights to the material.
   *  @param lights - a list containing all spotlights in the scene.
   */ 
  virtual void SetSpotlights(const std::vector<light::spotlight_info>& lights) = 0;

  virtual GLuint GetProgramDescriptor() = 0;
};

} // namespace shader
} // namespace monkeysworld

#endif