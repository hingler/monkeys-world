#ifndef BUTTON_MATERIAL_H_
#define BUTTON_MATERIAL_H_

#include <engine/Context.hpp>

#include <shader/ShaderProgram.hpp>
#include <shader/Material.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

struct ButtonMaterial : public Material {
  ButtonMaterial(engine::Context* ctx);

  void UseMaterial() override;

  glm::vec2 resolution;
  float border_width;
  float border_radius;
  glm::vec4 button_color;
  glm::vec4 border_color;

 private:
  ShaderProgram prog_;
};

}
}
}

#endif