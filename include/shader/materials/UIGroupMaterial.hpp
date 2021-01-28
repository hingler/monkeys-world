#ifndef UI_GROUP_MATERIAL_H_
#define UI_GROUP_MATERIAL_H_

#define TEXTURES_PER_CALL 4

#include <shader/Material.hpp>

#include <engine/Context.hpp>

#include <shader/ShaderProgram.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

class UIGroupMaterial : public Material {
 public:
  /**
   *  Creates the UIGroupMaterial.
   */ 
  UIGroupMaterial(engine::Context* ctx);

  /**
   *  Sets textures for this call.
   *  @param textures - array of textures sent to the material.
   */ 
  void SetTextures(GLuint textures[TEXTURES_PER_CALL]);

  /**
   *  Uses the underlying program.
   */ 
  void UseMaterial() override;
 private:
  ShaderProgram prog_;
  GLuint textures_[TEXTURES_PER_CALL];
};

}
}
}

#endif