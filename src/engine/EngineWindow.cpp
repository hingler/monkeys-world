#include <engine/EngineWindow.hpp>

namespace monkeysworld {
namespace engine {

EngineWindow::EngineWindow(Context* ctx) {
  root_ui_ = std::make_shared<critter::ui::UIGroup>(ctx);
  root_ui_->SetId(ID);
  root_ui_->SetPosition(glm::vec2(0, 0));
  
}

void EngineWindow::AddChild(std::shared_ptr<critter::ui::UIObject> obj) {
  root_ui_->AddChild(obj);
}

void EngineWindow::RemoveChild(uint64_t id) {
  if (id != ID) {
    root_ui_->RemoveChild(id);
  }
}

std::shared_ptr<critter::Object> EngineWindow::GetChild(uint64_t id) {
  if (id != ID) {
    return root_ui_->GetChild(id);
  }

  return nullptr;
}

std::vector<std::shared_ptr<critter::Object>> EngineWindow::GetChildren() {
  return std::move(root_ui_->GetChildren());
}

bool EngineWindow::HandleClickEvent(input::MouseEvent& e) {
  return root_ui_->HandleClickEvent(e);
}

glm::vec2 EngineWindow::GetDimensions() const {
  return root_ui_->GetDimensions();
}

void EngineWindow::RenderMaterial(const engine::RenderContext& rc) {
  root_ui_->RenderMaterial(rc);
}

void EngineWindow::Layout() {
  glm::ivec2 dims;
  auto ctx = root_ui_->GetContext();
  ctx->GetFramebufferSize(&dims.x, &dims.y);
  if (dims != win_size_) {
    root_ui_->SetDimensions(static_cast<glm::vec2>(dims));
    win_size_ = dims;
  }

  root_ui_->PreLayout();
}

std::shared_ptr<critter::ui::UIGroup> EngineWindow::GetRootObject() {
  return root_ui_;
}

}
}