#include <font/TextObject.hpp>
#include <critter/Visitor.hpp>

namespace monkeysworld {
namespace font {

using engine::Context;
using critter::Visitor;
using critter::GameObject;

TextObject::TextObject(engine::Context* ctx, const std::string& font_path)
  : GameObject(ctx), Text(font_path), mat(ctx) { }

void TextObject::Accept(critter::Visitor& v) {
  v.Visit(std::dynamic_pointer_cast<TextObject>(shared_from_this()));
}

void TextObject::PrepareAttributes() {
  GetGeometry()->PointToVertexAttribs();
}

void TextObject::RenderMaterial(const engine::RenderContext& rc) {
  // TODO: This can prob be consistent throughout the renderer
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  mat.SetModelTransforms(GetTransformationMatrix());
  mat.SetCameraTransforms(rc.GetActiveCamera().view_matrix);
  mat.SetGlyphTexture(GetTexture());
  mat.SetTextColor(GetTextColor());
  mat.UseMaterial();
  Draw();
}

void TextObject::Draw() {
  glDrawElements(GL_TRIANGLES, GetGeometry()->GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
}

}
}