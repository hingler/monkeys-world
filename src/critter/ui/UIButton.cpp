#include <critter/ui/UIButton.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {

using font::UITextObject;

UIButton::UIButton(engine::Context* ctx, const std::string& font_path) : UITextObject(ctx, font_path),
                                                                         button_color(glm::vec3(0.8), 1.0),
                                                                         border_color(glm::vec3(0.6), 1.0),
                                                                         border_width(2.0f),
                                                                         border_radius(5.0f),
                                                                         mat_(ctx) {
  {
    std::unique_lock<std::mutex>(mesh_mutex_);
    if (!(mesh_local_ = mesh_.lock())) {
      mesh_local_ = std::make_shared<model::Mesh<storage::PositionPacket>>();
      mesh_local_->AddVertex({ glm::vec3( -1,  1, 0 ) });
      mesh_local_->AddVertex({ glm::vec3( -1, -1, 0 ) });
      mesh_local_->AddVertex({ glm::vec3(  1, -1, 0 ) });
      mesh_local_->AddVertex({ glm::vec3(  1,  1, 0 ) });

      mesh_local_->AddPolygon(0, 1, 2);
      mesh_local_->AddPolygon(0, 2, 3);

      mesh_ = mesh_local_;
    }
  }
}

void UIButton::DrawUI(glm::vec2 xyMin, glm::vec2 xyMax) {
  mat_.resolution = GetDimensions();
  mat_.border_color = border_color;
  mat_.border_width = border_width;
  mat_.border_radius = border_radius;
  mat_.button_color = button_color;
  mat_.UseMaterial();
  mesh_local_->PointToVertexAttribs();
  glDrawElements(GL_TRIANGLES, mesh_local_->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
  UITextObject::DrawUI(xyMin, xyMax);
}


}
}
}