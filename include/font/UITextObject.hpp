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
class UITextObject : public critter::ui::UIObject {
 public:
  /**
   *  Constructs a new UITextObject.
   *  @param ctx - the context
   *  @param font_path - the path to the desired font.
   */ 
  UITextObject(engine::Context* ctx, const std::string& font_path);

  /**
   *  Modifies the font associated with this text object.
   */ 
  void SetFont(const std::string& font_path) {
    text_.SetFont(font_path);
  }

  void SetText(const std::string& text) {
    text_.SetText(text);
  }

  std::string GetText() {
    return text_.GetText();
  }

  void SetTextColor(const glm::vec4& col) {
    text_.SetTextColor(col);
  }

  glm::vec4 GetTextColor() {
    return text_.GetTextColor();
  }

  void SetTextSize(float size_pt) {
    text_.SetTextSize(size_pt);
  }

  float GetTextSize() {
    return text_.GetTextSize();
  }

  void SetHorizontalAlign(AlignmentH align) {
    TextFormat tx = text_.GetTextFormat();
    tx.horiz_align = align;
    text_.SetTextFormat(tx);
  }

  void DrawUI(glm::vec2 xMin, glm::vec2 xMax) override;

 private:
  shader::materials::TextMaterial mat_;
  Text text_;

};

}
}

#endif