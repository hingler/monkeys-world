#ifndef SKYBOX_MATERIAL_H_
#define SKYBOX_MATERIAL_H_

#include <shader/Material.hpp>
#include <shader/ShaderProgram.hpp>

#include <engine/Context.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

class SkyboxMaterial : public shader::Material {
 public:
  SkyboxMaterial(engine::Context* context);

  void UseMaterial() override;

  /**
   *  Sets the view matrix.
   */ 
  void SetCameraView(const glm::mat4& view_mat);

  /**
   *  Sets the perspective matrix.
   */ 
  void SetCameraPersp(const glm::mat4& model_mat);

  /**
   *  Sets the cube map.
   */ 
  void SetCubeMap(GLuint cube_map);

 private:
  glm::mat4 view_mat_;
  glm::mat4 model_mat_;
  GLuint cube_map_;
  
  ShaderProgram skybox_prog_;
};

}
}
}

#endif