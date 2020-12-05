#include <critter/Object.hpp>

namespace monkeysworld {
namespace critter {

utils::IDGenerator Object::id_generator_;
Object::Object(Context* ctx) {
  ctx_ = ctx;
  id_ = id_generator_.GetUniqueId();
}

uint64_t Object::GetId() {
  return id_;
}

Context* Object::GetContext() {
  return ctx_;
}

// non problem, but:
// if multiple methods perform some setup per-frame,
// how will we fare?

// a: use other methods to do that, and provide proper inheritance for those.
// ideally: we'll only override this once with actual ops.
void Object::Update() {
  // noop
}

void Object::Destroy() {
  // noop
}

Object::Object(const Object& other) {
  id_ = id_generator_.GetUniqueId();
  ctx_ = other.ctx_;
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
}

Object& Object::operator=(Object&& other) {
  id_ = other.id_;
  other.id_ = 0;
  ctx_ = other.ctx_;

  return *this;
}

}
}