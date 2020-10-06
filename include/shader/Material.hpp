#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <glm/glm.hpp>

namespace screenspacemanager {
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
   *  Passes transform data to the material's uniform
   *  The material should handle this data as it sees fit
   *  @param vp_matrix - View * Projection.
   */ 
  virtual void SetCameraTransforms(const glm::mat4& vp_matrix) = 0;

  
};

} // namespace shader
} // namespace screenspacemanager

#endif