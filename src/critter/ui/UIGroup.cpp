#include <critter/ui/UIGroup.hpp>


namespace monkeysworld {
namespace critter {
namespace ui {

using engine::Context;

typedef std::shared_ptr<UIObject> child_ptr;

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
  } else if (obj.get() == this) {
    // no self nesting!
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

void UIGroup::DrawUI(glm::vec2 min, glm::vec2 max, shader::Canvas canvas) {
  // note: framebuffer is bound if this is being called
  // plus, all of its children have already been drawn
  glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_textures_);
  UIGroupPacket p;
  std::shared_ptr<UIObject> child;
  glm::vec2 group_dims = GetDimensions();
  GLuint textures[4];
  float opacities[4];
  int current_child = 0;
  int index = 0;
  mesh_.Clear();
  
  // maintain sorted z-index order for children+
  std::sort(children_.begin(), children_.end(), [&](child_ptr a, child_ptr b) {
    return (a->z_index < b->z_index);
  });
  
  while (current_child < children_.size()) {
    // add shit to our mesh as we go
    // once we hit 4: draw it
    // if we're done, and index > 0: draw it
    child = children_[current_child];
    glm::vec2 min_coord = child->GetPosition();
    glm::vec2 max_coord = min_coord + child->GetDimensions();
    if (min_coord.x > group_dims.x || min_coord.y > group_dims.y || max_coord.x < 0 || max_coord.y < 0) {
      current_child++;
      continue;
    }

    textures[index] = child->GetFramebufferColor();
    opacities[index] = child->GetOpacity();
    p.index = static_cast<float>(index);

    auto pos = child->GetPosition();
    auto dims = GetDimensions();

    auto dims_child = child->GetDimensions();

    // top left
    p.pos.x = (pos.x / dims.x) * 2 - 1;
    p.pos.y = 1 - (pos.y / dims.y) * 2;


    p.texcoord = glm::vec2(0, 1);
    mesh_.AddVertex(p);

    // bottom left
    p.pos.y -= (dims_child.y / dims.y) * 2;
    p.texcoord = glm::vec2(0, 0);
    mesh_.AddVertex(p);

    // bottom right
    p.pos.x += (dims_child.x / dims.x) * 2;
    p.texcoord = glm::vec2(1, 0);
    mesh_.AddVertex(p);

    // top right
    p.pos.y += (dims_child.y / dims.y) * 2;
    p.texcoord = glm::vec2(1, 1);
    mesh_.AddVertex(p);

    mesh_.AddPolygon(4 * index, 4 * index + 1, 4 * index + 2);
    mesh_.AddPolygon(4 * index, 4 * index + 2, 4 * index + 3);

    current_child++;
    index++;

    if (index == 4) {
      mat_.SetTextures(textures);
      mat_.SetOpacity(opacities);
      mesh_.PointToVertexAttribs();
      mat_.UseMaterial();
      glDrawElements(GL_TRIANGLES, static_cast<int>(mesh_.GetIndexCount()), GL_UNSIGNED_INT, (void*)0);

      index = 0;
    }
  }

  if (index > 0) {
    mat_.SetTextures(textures);
    mat_.SetOpacity(opacities);
    mesh_.PointToVertexAttribs();
    mat_.UseMaterial();
    glDrawElements(GL_TRIANGLES, static_cast<int>(mesh_.GetIndexCount()), GL_UNSIGNED_INT, (void*)0);
  }
}

void UIGroup::UIGroupPacket::Bind() {
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(UIGroupPacket), (void*)0);
  glEnableVertexAttribArray(0);
  
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(UIGroupPacket), (void*)(sizeof(glm::vec2)));
  glEnableVertexAttribArray(1);

  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(UIGroupPacket), (void*)(2 * sizeof(glm::vec2)));
  glEnableVertexAttribArray(2);
}

}
}
}