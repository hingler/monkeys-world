#include <critter/ui/UIButton.hpp>
#include <audio/AudioManager.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {

using font::UITextObject;

std::weak_ptr<model::Mesh<storage::PositionPacket>> UIButton::mesh_;
std::mutex UIButton::mesh_mutex_;


UIButton::UIButton(engine::Context* ctx, const std::string& font_path) : UITextObject(ctx, font_path),
                                                                         button_color(glm::vec3(0.8), 1.0),
                                                                         border_color(glm::vec3(0.6), 1.0),
                                                                         border_width(2.0f),
                                                                         border_radius(5.0f),
                                                                         mat_(ctx),
                                                                         pressed_(false) {
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

  if (pressed_) {
    BOOST_LOG_TRIVIAL(trace) << "pressed";
    mat_.button_color *= 0.7;
    mat_.border_color *= 0.7;
    mat_.button_color.a = mat_.border_color.a = 1.0;
  }

  mat_.UseMaterial();
  mesh_local_->PointToVertexAttribs();
  glDrawElements(GL_TRIANGLES, mesh_local_->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
  glDisable(GL_DEPTH_TEST);
  UITextObject::DrawUI(xyMin, xyMax);
  glEnable(GL_DEPTH_TEST);
}

bool UIButton::OnClick(const input::MouseEvent& e) {
  BOOST_LOG_TRIVIAL(trace) << "button clicked";
  if (e.button == GLFW_MOUSE_BUTTON_LEFT) {
    switch (e.action) {
      case GLFW_PRESS:
        pressed_ = true;
        GetContext()->GetAudioManager()->AddFileToBuffer("resources/flap_jack_scream.ogg", audio::AudioFiletype::OGG);
        Invalidate();
        break;
      case GLFW_RELEASE:
        pressed_ = false;
        Invalidate();
        break;
    }

    return true;
  }

  return false;
}


}
}
}