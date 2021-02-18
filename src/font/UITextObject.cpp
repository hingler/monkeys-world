#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <storage/VertexPacketTypes.hpp>

#include <font/UITextObject.hpp>

namespace monkeysworld {
namespace font {

using storage::VertexPacket2D;

UITextObject::UITextObject(engine::Context* ctx, const std::string& font_path)
  : UIObject(ctx), mat_(ctx), text_(ctx, font_path) { 
    TextFormat format;
    format.char_spacing = 0;
    format.horiz_align = LEFT;
    format.vert_align = TOP;
    text_.SetTextFormat(format);
  }

void UITextObject::DrawUI(glm::vec2 xMin, glm::vec2 xMax, shader::Canvas canvas) {
  auto text_mesh = text_.GetGeometry();
  auto format = text_.GetTextFormat();

  glm::mat4 model_mat(1.0);
  glm::vec3 scale(GetScaleFactor(), 1.0);

  // scale text up to match window scale + correct aspect ratio

  // font scales on a base assumption -- the screen is 1920px.
  // this works fine for 3d, but in 2d we need to scale up.
  // use the width to scale fonts back up

  // iterate through mesh, recording min/max x and y coords
  // mesh is -1 to 1, we don't care about how it gets transformed
  // size of text in render is (clip width * (x width / 2), clip height * (y height / 2))
  // we could use a function call to access it
  // performance should be fine, just dont write an essay

  

  // translate so that our text lines up with top left corner of the framebuffer
  switch (format.horiz_align) {
    case LEFT:
      model_mat = glm::translate(model_mat, glm::vec3(-1.0, 0.0, 0.0));
      break;
    case RIGHT:
      model_mat = glm::translate(model_mat, glm::vec3(1.0, 0.0, 0.0));
      break;
    case CENTER:
      break;
  }

  switch (format.vert_align) {
    case TOP:
      model_mat = glm::translate(model_mat, glm::vec3(0.0, 1.0, 0.0));
      break;
    case MIDDLE:
      break;
    case BOTTOM:
      model_mat = glm::translate(model_mat, glm::vec3(0.0, -1.0, 0.0));
      break;
  }

  model_mat = glm::scale(model_mat, scale);

  text_mesh->PointToVertexAttribs();

  mat_.SetModelTransforms(model_mat);
  mat_.SetCameraTransforms(glm::mat4(1.0));
  mat_.SetGlyphTexture(text_.GetTexture());
  mat_.SetTextColor(text_.GetTextColor());
  mat_.UseMaterial();

  glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(text_mesh->GetIndexCount()), GL_UNSIGNED_INT, (void*)0);
}

glm::vec2 UITextObject::GetMinimumBoundingDims() const {
  if (text_.GetText().size() <= 0) {
    return glm::vec2(0);
  }
  
  auto geom_ = text_.GetGeometry();
  glm::vec2 min(100);
  glm::vec2 max(-100);
  const VertexPacket2D* data = geom_->GetVertexData();
  for (int i = 0; i < geom_->GetVertexCount(); i++) {
    min = glm::min(min, data->position);
    max = glm::max(max, data->position);
    data++;
  }

  glm::vec2 range(max.x - min.x, max.y - min.y);
  range *= GetScaleFactor();
  glm::vec2 fb_size = GetDimensions();
  glm::vec2 res(range.x * (fb_size.x / 2), range.y * (fb_size.y / 2));
  return res;
}

glm::vec2 UITextObject::GetScaleFactor() const {
  glm::ivec2 window_size;
  glm::vec2 text_fb_size = GetDimensions();
  GetContext()->GetFramebufferSize(&window_size.x, &window_size.y);

  glm::vec2 scale(1.0);
  scale.x = (window_size.x / text_fb_size.x);
  scale.y = (window_size.y / text_fb_size.y) * (static_cast<float>(window_size.x) / window_size.y);
  scale *= (960.0f / window_size.x);

  return scale;
}

}
}