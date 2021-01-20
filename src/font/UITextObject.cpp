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

  glm::mat4 model_mat(1.0);
  glm::ivec2 window_size;
  glm::vec2 text_fb_size = GetDimensions();
  GetContext()->GetFramebufferSize(&window_size.x, &window_size.y);
  glm::vec3 scale(1.0);

  // scale text up to match window scale + correct aspect ratio
  // font scales on a base assumption -- the screen is 1920px.
  // this works fine for 3d, but in 2d we need to scale up.
  // use the width to scale fonts back up

  scale.x = (window_size.x / text_fb_size.x);
  scale.y = (window_size.y / text_fb_size.y) * (static_cast<float>(window_size.x) / window_size.y);
  scale *= (960.0f / window_size.x);
  
  float y_max = 0.0f;

  int i = 0;
  for (const VertexPacket2D* c = text_mesh->GetVertexData(); i < text_mesh->GetVertexCount(); c++, i++) {
    y_max = (y_max > c->position.y ? y_max : c->position.y);
  }

  y_max *= scale.y;

  // translate so that our text lines up with top left corner of the framebuffer
  model_mat = glm::translate(model_mat, glm::vec3(-1.0, 1.0 - y_max, 0.0));
  model_mat = glm::scale(model_mat, scale);

  text_mesh->PointToVertexAttribs();

  mat_.SetModelTransforms(model_mat);
  mat_.SetCameraTransforms(glm::mat4(1.0));
  mat_.SetGlyphTexture(GetTexture());
  mat_.SetTextColor(GetTextColor());
  mat_.UseMaterial();

  glDrawElements(GL_TRIANGLES, text_mesh->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
}

}
}