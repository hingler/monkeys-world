#ifndef SKY_BOX_MATERIAL_H_
#define SKY_BOX_MATERIAL_H_

#include <engine/Context.hpp>

#include <shader/Material.hpp>
#include <shader/ShaderProgram.hpp>

#include <shader/CubeMap.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

class SkyBoxMaterial : public ::monkeysworld::shader::Material {
 public:
  SkyBoxMaterial(engine::Context* ctx);

  void UseMaterial() override;

  void SetViewMatrix(const glm::mat4& view_matrix);
  void SetPerspectiveMatrix(const glm::mat4& perspective_matrix);
  void SetCubeMap(CubeMap* map);
  GLuint GetProgramDescriptor() override;

  void SetLights(const std::vector<light::LightData>& lights) override {};
  void SetSpotlights(const std::vector<light::spotlight_info>& lights) override {};
 private:
  ShaderProgram skybox_prog_;
  glm::mat4 view_mat_;
  glm::mat4 pers_mat_;
  GLuint skybox_cubemap_;
};

}
}
}


#endif