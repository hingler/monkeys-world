#include <engine/Scene.hpp>
#include <critter/Empty.hpp>

namespace monkeysworld {
namespace engine {

Scene::Scene() {}

void Scene::CreateScene(Context* ctx) {
  game_root_ = std::make_shared<critter::Empty>(ctx);
  ui_window_ = std::make_shared<EngineWindow>(ctx);
  ui_window_->GetRootObject()->Invalidate();
  Initialize(ctx);
}

std::shared_ptr<critter::GameObject> Scene::GetGameObjectRoot() {
  return game_root_;
}

std::shared_ptr<critter::ui::Window> Scene::GetWindow() {
  return ui_window_;
}

}
}