#include <font/Text.hpp>

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace font {

Text::Text(engine::Context* ctx, const std::string& font_path) {
  ctx_ = ctx;
  font_ = ctx_->GetCachedFileLoader()->LoadFont(font_path);
  color_ = glm::vec4(glm::vec3(0.0), 1.0);
  size_ = 24.0f;
  text_ = "";
  mesh_valid_ = false;
}

void Text::SetFont(const std::string& font_path) {
  font_ = ctx_->GetCachedFileLoader()->LoadFont(font_path);
  mesh_valid_ = false;
}

std::shared_ptr<const Font> Text::GetFont() {
  return font_;
}

void Text::SetText(const std::string& text) {
  text_ = text;
  mesh_valid_ = false;
}

std::string Text::GetText() {
  return text_;
}

void Text::SetTextColor(const glm::vec4& col) {
  color_ = col;
}

glm::vec4 Text::GetTextColor() {
  return color_;
}

void Text::SetTextSize(float size_pt) {
  mesh_valid_ = false;
  size_ = size_pt;
}

float Text::GetTextSize() {
  return size_;
}

std::shared_ptr<model::Mesh<storage::VertexPacket2D>> Text::GetGeometry() {
  if (!mesh_valid_) {
    mesh_ = font_->GetTextGeometry(text_, size_);
    mesh_valid_ = true;
  }

  return mesh_;
}

GLuint Text::GetTexture() {
  return font_->GetGlyphAtlas();
}

}
}