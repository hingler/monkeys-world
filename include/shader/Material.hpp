#ifndef MATERIAL_H_
#define MATERIAL_H_

#include <shader/light/LightDataTemp.hpp>

#include <glm/glm.hpp>

#include <vector>

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
   *  Passes the camera matrix to the material's uniform
   *  The material should handle this data as it sees fit
   *  @param vp_matrix - View * Projection.
   */ 
  virtual void SetCameraTransforms(const glm::mat4& vp_matrix) = 0;

  /**
   *  Passes model transform data.
   *  @param model_matrix - Matrix used to transform model.
   */ 
  virtual void SetModelTransforms(const glm::mat4& model_matrix) = 0;

  /**
   *  Passes light data to the material.
   *  @param lights - a list containing all lights in the scene, in global coords.
   */ 
  virtual void SetLights(const std::vector<light::LightData>& lights) = 0;
};

} // namespace shader
} // namespace screenspacemanager

#endif