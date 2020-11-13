#include <critter/GameObject.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <memory>

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace critter {

using utils::IDGenerator;
IDGenerator GameObject::id_generator_;

GameObject::GameObject() {
  this->parent_ = nullptr;
  this->dirty_ = true;
  this->position = glm::vec3(0);
  this->rotation = glm::vec3(0);
  this->scale = glm::vec3(1);
  this->id_ = id_generator_.GetUniqueId();
}

void GameObject::AddChild(std::shared_ptr<GameObject> child) {
  // if the child is a parent (direct or indirect) this will fail.
  if (child->GetChild(this->GetId()) != NULL) {
    return;
  }

  BOOST_LOG_TRIVIAL(trace) << "Adding child with ID " << child->GetId();

  // if the child is already a child (direct or indirect) it will be removed.
  if (child->parent_ != nullptr) {
    child->parent_->RemoveChild(child->GetId());
  }
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
    if (result != nullptr) {
      return result;
    }
  }

  return nullptr;
}

GameObject* GameObject::GetParent() {
  return parent_;
}

uint64_t GameObject::GetId() {
  return id_;
}

void GameObject::SetPosition(const glm::vec3& new_pos) {
  dirty_.store(true, std::memory_order_release);
  position = new_pos;
}

void GameObject::SetRotation(const glm::vec3& new_rot) {
  dirty_.store(true, std::memory_order_release);
  rotation = new_rot;
}

void GameObject::SetScale(const glm::vec3& new_scale) {
  dirty_.store(true, std::memory_order_release);
  scale = new_scale;
}

glm::mat4 GameObject::GetTransformationMatrix() {
  if (dirty_) {
    tf_matrix_cache_ = glm::mat4(1.0);
    // scales, then rotates, then translates
    tf_matrix_cache_ = glm::translate(tf_matrix_cache_, position);
    tf_matrix_cache_ *= glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
    tf_matrix_cache_ = glm::scale(tf_matrix_cache_, scale);
  }

  if (parent_ != nullptr) {
    return parent_->GetTransformationMatrix() * tf_matrix_cache_;
  }

  return tf_matrix_cache_;
}

void GameObject::RemoveChild(uint64_t id) {
  for (auto child : children_) {
    if (child->GetId() == id) {
      children_.erase(child);
      return;
    }
  }

  // check descendants
  for (auto child : children_) {
    child->RemoveChild(id);
  }
}

} // namespace critter
} // namespace monkeysworld