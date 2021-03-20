#include <critter/Object.hpp>
#include <critter/Visitor.hpp>

namespace monkeysworld {
namespace critter {

utils::IDGenerator Object::id_generator_;
Object::Object(engine::Context* ctx) {
  ctx_ = ctx;
  id_ = id_generator_.GetUniqueId();
  created_ = false;
}

uint64_t Object::GetId() {
  return id_;
}

void Object::SetId(uint64_t new_id) {
  id_generator_.RegisterUniqueId(new_id);
  id_ = new_id;
}

engine::Context* Object::GetContext() const {
  return ctx_;
}

void Object::UpdateFunc() {
  if (!created_) {
    Create();
    created_ = true;
  }
  
  Update();
}

void Object::Create() {
  // noop
}

void Object::Update() {
  // noop
}

void Object::Destroy() {
  // noop
}

Object::Object(const Object& other) {
  id_ = id_generator_.GetUniqueId();
  ctx_ = other.ctx_;
  created_ = false;
}

Object& Object::operator=(const Object& other) {
  id_ = id_generator_.GetUniqueId();
  ctx_ = other.ctx_;
  return *this;
}

Object::Object(Object&& other) {
  id_ = other.id_;
  other.id_ = 0;
  ctx_ = other.ctx_;
  created_ = false;
}

Object& Object::operator=(Object&& other) {
  id_ = other.id_;
  other.id_ = 0;
  ctx_ = other.ctx_;

  return *this;
}

}
}