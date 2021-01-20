#ifndef UI_TEXT_OBJECT_H_
#define UI_TEXT_OBJECT_H_

#include <font/Text.hpp>
#include <critter/ui/UIObject.hpp>

#include <shader/materials/TextMaterial.hpp>


namespace monkeysworld {
namespace font {

/**
 *   Renders text onto the UI.
 */ 
class UITextObject : public critter::ui::UIObject, public Text {
 public:
  /**
   *  Constructs a new UITextObject.
   *  @param ctx - the context
   *  @param font_path - the path to the desired font.
   */ 
  UITextObject(engine::Context* ctx, const std::string& font_path);
  void Accept(critter::Visitor& v) override;
  void DrawUI(glm::vec2 xMin, glm::vec2 xMax) override;

 private:
  shader::materials::TextMaterial mat_;

};

}
}

#endif