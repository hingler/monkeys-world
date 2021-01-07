#ifndef TEXT_OBJECT_H_
#define TEXT_OBJECT_H_

#include <glm/glm.hpp>

#include <engine/Context.hpp>
#include <engine/RenderContext.hpp>
#include <critter/GameObject.hpp>

#include <model/Mesh.hpp>
#include <storage/VertexPacketTypes.hpp>

#include <shader/materials/TextMaterial.hpp>

#include <font/Text.hpp>

namespace monkeysworld {
namespace font {

// TODO: choose a better name maybe :(
class TextObject : public critter::GameObject, public Text {

 public:
  /**
   *  Constructs a new text object, associated with a given font.
   *  @param ctx - the context associated with this object.
   *  @param font_path - the path to the desired font to be rendered.
   */ 
  TextObject(engine::Context* ctx, const std::string& font_path);

  /**
   *  Accept visitor.
   */ 
  void Accept(critter::Visitor& v) override;

  void PrepareAttributes() override;
  void RenderMaterial(const engine::RenderContext& rc) override;
  void Draw() override;
 private:
  shader::materials::TextMaterial mat;

  
};

}
}

#endif