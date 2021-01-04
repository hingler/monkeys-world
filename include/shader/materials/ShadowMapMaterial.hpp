#ifndef SHADOW_MAP_MATERIAL_H_
#define SHADOW_MAP_MATERIAL_H_

#include <file/FileLoader.hpp>

#include <shader/Material.hpp>
#include <shader/ShaderProgram.hpp>


#include <critter/Context.hpp>

#include <glm/glm.hpp>

#include <memory>

namespace monkeysworld {
namespace shader {
namespace materials {

class ShadowMapMaterial : public ::monkeysworld::shader::Material {
 public:
  /**
   *  Creates a new ShadowMapMaterial.
   *  @param ctx - Context ptr.
   */ 
  ShadowMapMaterial(critter::Context* ctx);

  void UseMaterial() override;

  void SetCameraTransforms(const glm::mat4& vp_matrix) override;
  void SetModelTransforms(const glm::mat4& model_matrix) override;
  GLuint GetProgramDescriptor() override;

  // nop
  void SetLights(const std::vector<light::LightData>& lights) override {};
  void SetSpotlights(const std::vector<light::spotlight_info>& lights) override {};

 private:
  ShaderProgram shadow_prog_;
};

}
}
}

#endif