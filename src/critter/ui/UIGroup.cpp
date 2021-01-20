#include <critter/ui/UIGroup.hpp>


namespace monkeysworld {
namespace critter {
namespace ui {

using engine::Context;

UIGroup::UIGroup(Context* ctx) : UIObject(ctx), mat_(ctx) { 
  max_textures_ = -1;
}

std::shared_ptr<Object> UIGroup::GetChild(uint64_t id) {
  for (auto child : children_) {
    if (id == child->GetId()) {
      return child;
    }
  }

  for (auto child : children_) {
    if (auto res = child->GetChild(id)) {
      return res;
    }
  }

  return std::shared_ptr<Object>(nullptr);
}

std::vector<std::shared_ptr<Object>> UIGroup::GetChildren() {
  std::vector<std::shared_ptr<Object>> res;
  for (auto child : children_) {
    res.push_back(child);
  }

  return res;
}

void UIGroup::AddChild(std::shared_ptr<UIObject> obj) {
  if (obj->GetChild(this->GetId()) != NULL) {
    // bad nesting!
    return;
  }

  if (auto parent = obj->GetParent()) {
    auto ui_parent = std::static_pointer_cast<UIGroup>(parent);
    ui_parent->RemoveChild(obj->GetId());
  }

  obj->parent_ = std::weak_ptr<UIObject>(this->shared_from_this());
  children_.push_back(obj);
}

void UIGroup::RemoveChild(uint64_t id) {
  for (auto ptr = children_.begin(); ptr != children_.end(); ptr++) {
    if ((*ptr)->GetId() == id) {
      children_.erase(ptr);
      return;
    }
  }

  for (auto child : children_) {
    if (auto group = std::dynamic_pointer_cast<UIGroup>(child)) {
      group->RemoveChild(id);
      return;
    }
  }
}

void UIGroup::DrawUI(glm::vec2 min, glm::vec2 max) {
  // note: framebuffer is bound if this is being called
  // plus, all of its children have already been drawn
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_textures_);
  BOOST_LOG_TRIVIAL(trace) << max_textures_ << " texture units available";
  model::Mesh<UIGroupPacket> mesh;
  UIGroupPacket p;
  std::shared_ptr<UIObject> child;
  GLuint textures[4];
  for (int i = 0; i < children_.size(); i += TEXTURES_PER_CALL) {
    for (int j = 0; j < TEXTURES_PER_CALL && (i + j) < children_.size(); j++) {
      child = children_[i + j];
      
      // tba: pass textures to material
      textures[j] = child->GetFramebufferColor();
      p.index = j;

      // top left
      p.pos = ((child->GetPosition()) / GetDimensions());
      p.pos *= 2;
      p.pos.x -= 1;
      p.pos.y -= 1;


      p.texcoord = glm::vec2(0);
      mesh.AddVertex(p);

      // bottom left
      p.pos.y += (child->GetDimensions().y / GetDimensions().y) * 2;
      p.texcoord = glm::vec2(0, 1);
      mesh.AddVertex(p);

      // bottom right
      p.pos.x += (child->GetDimensions().x / GetDimensions().x) * 2;
      p.texcoord = glm::vec2(1, 1);
      mesh.AddVertex(p);

      // top right
      p.pos.y -= (child->GetDimensions().y / GetDimensions().y) * 2;
      p.texcoord = glm::vec2(1, 0);
      mesh.AddVertex(p);

      mesh.AddPolygon(0, 1, 2);
      mesh.AddPolygon(0, 2, 3);
      // position / fb size
      // (position + dimensions) / fb size
      // tex coords always 0 - 1
      // index defined by the order we get to it in
    }

    mesh_ = mesh;
    mat_.SetTextures(textures);

    // point attribs
    mesh_.PointToVertexAttribs();
    mat_.UseMaterial();

    glDrawElements(GL_TRIANGLES, mesh_.GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
  }
}

void UIGroup::UIGroupPacket::Bind() {
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIGroupPacket), (void*)0);
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIGroupPacket), (void*)(sizeof(glm::vec2)));
  glEnableVertexAttribArray(1);

  glVertexAttribIPointer(2, 1, GL_UNSIGNED_SHORT, sizeof(UIGroupPacket), (void*)(2 * sizeof(glm::vec2)));
  glEnableVertexAttribArray(2);
}

}
}
}