#ifndef ENGINE_WINDOW_H_
#define ENGINE_WINDOW_H_

#include <critter/ui/Window.hpp>
#include <critter/ui/UIGroup.hpp>

#include <engine/Context.hpp>

namespace monkeysworld {
namespace engine {

class EngineWindow : public critter::ui::Window {

 public:
  EngineWindow(Context* ctx);
  void AddChild(std::shared_ptr<critter::ui::UIObject> obj) override;
  void RemoveChild(uint64_t id) override;
  std::shared_ptr<critter::Object> GetChild(uint64_t id) override;
  std::vector<std::shared_ptr<critter::Object>> GetChildren() override;
  bool HandleClickEvent(input::MouseEvent& e) override;
  glm::vec2 GetDimensions() const override;
  void RenderMaterial(const engine::RenderContext& rc) override;

  /**
   *  Equivalent to calling PreLayout on the underlying ui group.
   *  When our window size changes, this helps reorient children.
   *  @param size - the new size of this window.
   */ 
  void Layout();

  /**
   *  @returns the underlying root object.
   */ 
  std::shared_ptr<critter::ui::UIGroup> GetRootObject();

 private:
  std::shared_ptr<critter::ui::UIGroup> root_ui_;
  glm::ivec2 win_size_;

};

}
}

#endif