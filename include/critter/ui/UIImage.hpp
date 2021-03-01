#ifndef UI_IMAGE_H_
#define UI_IMAGE_H_

#include <critter/ui/UIObject.hpp>
#include <shader/Texture.hpp>

#include <shader/materials/TextureXferMaterial.hpp>

#include <memory>

namespace monkeysworld {
namespace critter {
namespace ui {

class UIImage : public UIObject {
 public:
  /**
   *  Creates a new UIImage.
   *  @param ctx - the currently active context.
   *  @param image_path - the path to the image which should be displayed on this object.
   */ 
  UIImage(engine::Context* ctx, const std::string& image_path);

  /**
   *  Method for drawing the image onto the screen.
   *  @param canvas - canvas object.
   *  TODO: Setup canvas so that it can draw images from a texture.
   */ 
  void DrawUI(glm::vec2, glm::vec2, shader::Canvas canvas);
 private:
  std::shared_ptr<const shader::Texture> tex_;
  shader::materials::TextureXferMaterial mat_;
};

}
}
}

#endif