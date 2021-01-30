#ifndef UI_BUTTON_H_
#define UI_BUTTON_H_

#include <critter/ui/UIObject.hpp>
#include <font/Text.hpp>

#include <model/Mesh.hpp>

#include <shader/materials/ButtonMaterial.hpp>
#include <shader/materials/TextMaterial.hpp>

// before creating this button

// create a mouse event listener
// send mouse events down the ui object chain
// create a cursor that exposes the current cursor position per request

// create a button material with rounded corners

namespace monkeysworld {
namespace critter {
namespace ui {

/**
 *  A simple template for creating a UI button.
 */ 
class UIButton : public UIObject {
 public:
  /**
   *  Creates a new UIButton.
   *  @param ctx - the context
   *  @param font_path - path to the desired font.
   */ 
  UIButton(engine::Context* ctx, const std::string& font_path);

  /**
   *  Updates the font associated with this button, and invalidates this component.
   *  @param font_name - path to a valid font.
   *                     Throws an exception if the path provided is not valid.
   */ 
  void SetFont(const std::string& font_name);

  /**
   *  Updates the text displayed on this button, and invalidates the component.
   *  @param text - the new text for this button.
   */ 
  void SetText(const std::string& text);

  /**
   *  Updates the size, of the text contained in this button. Invalidates the component.
   *  @param size_px - the size of the text, in px.
   *                   behavior is undefined if less than 0.
   */ 
  void SetTextSize(float size_px);

  /**
   *  Updates the color of the text contained in this button. Invalidates the component.
   *  @param text_color - the new text color for this button.
   *                      behavior is undefined if any fields fall outside the range [0, 1]
   */ 
  void SetTextColor(const glm::vec4& text_color);

  /**
   *  Updates the color of the button. Invalidates the component.
   *  @param button_color - the new button color for this button.
   *                        behavior is undefined if fields fall outside the range [0, 1]
   */ 
  void SetButtonColor(const glm::vec4& button_color);

  /**
   *  @param border_width - the new width for the border.
   *                        behavior undefined if < 0.
   */ 
  void SetBorderWidth(float border_width);

  /**
   *  @param border_color - new border color.
   */ 
  void SetBorderColor(const glm::vec4& border_color);

  /**
   *  @param border_radius - new border radius.
   */ 
  void SetBorderRadius(float border_radius);

  void DrawUI(glm::vec2 minXY, glm::vec2 maxXY) override;

 private:
  // color of text, RGBA
  glm::vec4 text_color_;

  font::Text text_;

  std::shared_ptr<model::Mesh<storage::PositionPacket>> mesh_local_;

  shader::materials::TextMaterial tex_mat_;
  shader::materials::ButtonMaterial butt_mat_;  // haha
  
  
  static std::weak_ptr<model::Mesh<storage::PositionPacket>> mesh_;
  static std::mutex mesh_mutex_;
  
};

}
}
}

#endif