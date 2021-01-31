#ifndef UI_BUTTON_H_
#define UI_BUTTON_H_

#include <font/UITextObject.hpp>
#include <shader/materials/ButtonMaterial.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {

class UIButton : public font::UITextObject {
 public:
  UIButton(engine::Context* ctx, const std::string& font_path);

  glm::vec4 button_color;
  glm::vec4 border_color;
  float border_width;
  float border_radius;

  void DrawUI(glm::vec2 xyMin, glm::vec2 xyMax) override;
 private:
  shader::materials::ButtonMaterial mat_;

  std::shared_ptr<model::Mesh<storage::PositionPacket>> mesh_local_;

  // contains a static mesh which contains a full-screen quad
  static std::weak_ptr<model::Mesh<storage::PositionPacket>> mesh_;
  static std::mutex mesh_mutex_;
};

}
}
}

#endif