#include <critter/ui/UIImage.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {

UIImage::UIImage(engine::Context* ctx, const std::string& image_path) : UIObject(ctx), mat_(ctx) {
  tex_ = GetContext()->GetCachedFileLoader()->LoadTexture(image_path);
}

void UIImage::DrawUI(glm::vec2, glm::vec2, shader::Canvas canvas) {
  mat_.SetTexture(tex_->GetTextureDescriptor());
  mat_.SetOpacity(GetOpacity());
  mat_.UseMaterial();
  DrawFullscreenQuad();
}

}
}
}