#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <storage/VertexPacketTypes.hpp>

#include <font/UITextObject.hpp>

namespace monkeysworld {
namespace font {

using storage::VertexPacket2D;

UITextObject::UITextObject(engine::Context* ctx, const std::string& font_path)
  : Text(ctx, font_path), UIObject(ctx), mat_(ctx) { }

void UITextObject::DrawUI(glm::vec2 xMin, glm::vec2 xMax) {
  auto text_mesh = GetGeometry();

  glm::mat4 model_mat;
  glm::ivec2 window_size;
  glm::vec2 text_fb_size = GetDimensions();
  GetContext()->GetFramebufferSize(&window_size.x, &window_size.y);
  glm::vec3 scale(1.0);

  // scale text up to match window scale + correct aspect ratio
  // fb size should correct to window aspect ratio -- just correct for window aspect ratio
  scale.x = (window_size.x / text_fb_size.x) * (static_cast<float>(window_size.x), window_size.y);
  scale.y = (window_size.y / text_fb_size.y);
  float y_max = 0.0f;
  for (const VertexPacket2D* c = text_mesh->GetVertexData(), int i = 0; i < text_mesh->GetVertexCount(); c++, i++) {
    y_max = (y_max < c->position.y ? y_max : c->position.y);
  }

  // translate so that our text lines up with top left corner of the framebuffer
  model_mat = glm::translate(model_mat, glm::vec3(-1.0, 1.0 - y_max, 0.0));
  model_mat = glm::scale(model_mat, scale);

  text_mesh->PointToVertexAttribs();

  mat_.SetModelTransforms(model_mat);
  mat_.SetCameraTransforms(glm::mat4());
  mat_.SetGlyphTexture(GetFont()->GetGlyphAtlas());
  mat_.SetTextColor(GetTextColor());
  mat_.UseMaterial();

  glDrawElements(GL_TRIANGLES, GetGeometry()->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
}

}
}