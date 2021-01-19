#ifndef UI_GROUP_H_
#define UI_GROUP_H_

#include <critter/ui/UIObject.hpp>
#include <model/Mesh.hpp>

#include <shader/materials/UIGroupMaterial.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {

/**
 *  UIGroups are the parts which actually contain other components.
 */ 
class UIGroup : public UIObject {
 public:
  UIGroup(engine::Context* ctx);


  std::shared_ptr<Object> GetChild(uint64_t id) override;
  std::vector<std::shared_ptr<Object>> GetChildren() override;

  /**
   *  Adds a child to this UIGroup.
   *  @param obj - child we are adding.
   */ 
  void AddChild(std::shared_ptr<UIObject> obj);

  /**
   *  Removes a child from this UIGroup.
   *  @param id - the id of the child we are removing.
   */ 
  void RemoveChild(uint64_t id);

  /**
   *  Uses DrawUI to draw all children to the screen.
   */ 
  void DrawUI(glm::vec2 min, glm::vec2 max) override;

 private:
  struct UIGroupPacket {
    glm::vec2 pos;            // vertex positions
    glm::vec2 texcoord;       // texture coordinates
    float index;              // index of child being drawn

    static void Bind();
  };

  std::vector<std::shared_ptr<UIObject>> children_;   // children of this layer
  model::Mesh<UIGroupPacket> mesh_;                   // group mesh
  GLint max_textures_;
  shader::materials::UIGroupMaterial mat_;
  
};

}
}
}

#endif