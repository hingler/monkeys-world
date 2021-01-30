#include <critter/ui/UIButton.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {

using font::TextFormat;
using font::AlignmentH;
using font::AlignmentV;

UIButton::UIButton(engine::Context* ctx, const std::string& font_path) : UIObject(ctx),
                                                                         text_(ctx, font_path),
                                                                         text_color_(0, 0, 0, 1),
                                                                         tex_mat_(ctx),
                                                                         butt_mat_(ctx) {
  TextFormat f;
  f.char_spacing = 0;
  f.horiz_align = AlignmentH::CENTER;
  f.vert_align = AlignmentV::MIDDLE;
  text_.SetTextFormat(f);
  
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

void UIButton::SetFont(const std::string& font_name) {
  text_.SetFont(font_name);
  Invalidate();
}

void UIButton::SetText(const std::string& text) {
  text_.SetText(text);
  Invalidate();
}

void UIButton::SetTextSize(float size_px) {
  if (size_px > 0) {
    text_.SetTextSize(size_px);
    Invalidate();
  }
}

void UIButton::SetTextColor(const glm::vec4& text_color) {
  float max_col = std::max(std::max(text_color.r, text_color.g), std::max(text_color.b, text_color.a));
  float min_col = std::min(std::min(text_color.r, text_color.g), std::min(text_color.b, text_color.a));
  if (min_col >= 0 && max_col <= 1) {
    text_color_ = text_color;
    Invalidate();
  }
}

void UIButton::SetButtonColor(const glm::vec4& button_color) {
  float max_col = std::max(std::max(button_color.r, button_color.g), std::max(button_color.b, button_color.a));
  float min_col = std::min(std::min(button_color.r, button_color.g), std::min(button_color.b, button_color.a));
  if (min_col >= 0 && max_col <= 1) {
    butt_mat_.button_color = button_color;
    Invalidate();
  }
}

void UIButton::SetBorderWidth(float border_width) {
  if (border_width > 0) {
    butt_mat_.border_width = border_width;
    Invalidate();
  }
}

void UIButton::SetBorderColor(const glm::vec4& border_color) {
  float max_col = std::max(std::max(border_color.r, border_color.g), std::max(border_color.b, border_color.a));
  float min_col = std::min(std::min(border_color.r, border_color.g), std::min(border_color.b, border_color.a));
  if (min_col >= 0 && max_col <= 1) {
    butt_mat_.border_color = border_color;
  }
}

void UIButton::SetBorderRadius(float border_radius) {
  if (border_radius > 0) {
    butt_mat_.border_radius = border_radius;
  }
}

void UIButton::DrawUI(glm::vec2 minXY, glm::vec2 maxXY) {
  // how to create mesh?
  // two draw calls
  // one for button, one for text on top
  butt_mat_.resolution = GetDimensions();
  butt_mat_.UseMaterial();
  mesh_local_->PointToVertexAttribs();
  glDrawElements(GL_TRIANGLES, mesh_local_->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);

  // draw text
  auto text_mesh = text_.GetGeometry();

  glm::mat4 model_mat(1.0);
  glm::ivec2 window_size;
  glm::vec2 text_fb_size = GetDimensions();
  GetContext()->GetFramebufferSize(&window_size.x, &window_size.y);
  glm::vec3 scale(1.0);

  scale.x = (window_size.x / text_fb_size.x);
  scale.y = (window_size.y / text_fb_size.y) * static_cast<float>(window_size.x) / window_size.y;
  scale *= (960.0f / window_size.x);

  model_mat = glm::scale(model_mat, scale);

  tex_mat_.SetTextColor(text_color_);
  tex_mat_.SetGlyphTexture(text_.GetTexture());
  tex_mat_.SetCameraTransforms(glm::mat4(1.0));
  tex_mat_.SetModelTransforms(model_mat);
  tex_mat_.UseMaterial();

  text_mesh->PointToVertexAttribs();
  glDrawElements(GL_TRIANGLES, text_mesh->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
}


}
}
}