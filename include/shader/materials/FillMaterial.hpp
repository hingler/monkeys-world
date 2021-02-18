#ifndef FILL_MATERIAL_H_
#define FILL_MATERIAL_H_

#include <engine/Context.hpp>
#include <shader/Material.hpp>
#include <shader/ShaderProgram.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

class FillMaterial : public ::monkeysworld::shader::Material {
 public:
  FillMaterial(engine::Context* context);
  void UseMaterial() override;
  
  /**
   *  Set the color associated with this material.
   *  @param col - the desired color, in RGBA.
   */ 
  void SetColor(const glm::vec4& col);
 private:
  ShaderProgram fill_prog_;
  glm::vec4 color_cache_;
};

}
}
}

#endif