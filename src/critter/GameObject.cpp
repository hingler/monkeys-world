#include <critter/GameObject.hpp>

#include <memory>

namespace monkeysworld {
namespace critter {

void GameObject::AddChild(std::shared_ptr<GameObject> child) {
  // invalid -- we get stuck in a looping dependency
  if (child->GetChild(this->GetId()) != NULL) {
    return;
  }

  child->parent_->RemoveChild(child->GetId());
  child->parent_ = this;
  // child is moved here -- don't want it in multiple locations
  children_.insert(child);
}

GameObject* GameObject::GetChild(uint64_t id) {
  // direct nesting will probably be common
  for (auto child : children_) {
    if (child->GetId() == id) {
      return child.get();
    }
  }

  // check for multiple nesting levels
  GameObject* result;
  for (auto child : children_) {
    result = child->GetChild(id);
    if (result != NULL) {
      return result;
    }
  }

  return NULL;
}

GameObject* GameObject::GetParent() {
  return parent_;
}

} // namespace critter
} // namespace monkeysworld