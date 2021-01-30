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

  /**
   *  Sets the text being displayed.
   *  @param text - new text
   */ 
  void SetText(const std::string& text) {
    text_.SetText(text);
  }

  /**
   *  @returns the text associated with this object.
   */ 
  std::string GetText() {
    return text_.GetText();
  }

  // get/set text color

  void SetTextColor(const glm::vec4& col) {
    text_.SetTextColor(col);
  }

  glm::vec4 GetTextColor() {
    return text_.GetTextColor();
  }

  // get/set text size

  void SetTextSize(float size_pt) {
    text_.SetTextSize(size_pt);
  }

  float GetTextSize() {
    return text_.GetTextSize();
  }

  /**
   *  Sets the horizontal alignment of this text object.
   *  @param align - the new desired alignment.
   */ 
  void SetHorizontalAlign(AlignmentH align) {
    TextFormat tx = text_.GetTextFormat();
    tx.horiz_align = align;
    text_.SetTextFormat(tx);
  }

  void SetVerticalAlign(AlignmentV align) {
    TextFormat tx = text_.GetTextFormat();
    tx.vert_align = align;
    text_.SetTextFormat(tx);
  }

  /**
   *  Draws text to screen.
   */ 
  void DrawUI(glm::vec2 xMin, glm::vec2 xMax) override;

 private:
  shader::materials::TextMaterial mat_;
  Text text_;

};

}
}

#endif