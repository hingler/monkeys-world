#include <font/TextObject.hpp>
#include <critter/Visitor.hpp>

namespace monkeysworld {
namespace font {

using engine::Context;
using critter::Visitor;
using critter::GameObject;

TextObject::TextObject(engine::Context* ctx, const std::string& font_path)
  : GameObject(ctx), Text(ctx, font_path), mat(ctx) { }

void TextObject::Accept(critter::Visitor& v) {
  v.Visit(std::dynamic_pointer_cast<TextObject>(shared_from_this()));
}

void TextObject::PrepareAttributes() {
  GetGeometry()->PointToVertexAttribs();
}

void TextObject::RenderMaterial(const engine::RenderContext& rc) {
  // TODO: manage blend modes efficiently
  mat.SetModelTransforms(GetTransformationMatrix());
  mat.SetCameraTransforms(rc.GetActiveCamera().vp_matrix);
  mat.SetGlyphTexture(GetTexture());
  mat.SetTextColor(GetTextColor());
  mat.UseMaterial();
  Draw();
}

void TextObject::Draw() {
  glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(GetGeometry()->GetIndexCount()), GL_UNSIGNED_INT, (void*)0);
}

}
}