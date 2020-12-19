#include <critter/Object.hpp>
#include <critter/Visitor.hpp>

namespace monkeysworld {
namespace critter {

utils::IDGenerator Object::id_generator_;
Object::Object() {
  id_ = id_generator_.GetUniqueId();
}

void Object::Accept(Visitor& v) {
  v.Visit(this);
}

uint64_t Object::GetId() {
  return id_;
}

void Object::SetId(uint64_t new_id) {
  id_generator_.RegisterUniqueId(new_id);
  id_ = new_id;
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
}

Object& Object::operator=(const Object& other) {
  id_ = id_generator_.GetUniqueId();
  return *this;
}

Object::Object(Object&& other) {
  id_ = other.id_;
  other.id_ = 0;
}

Object& Object::operator=(Object&& other) {
  id_ = other.id_;
  other.id_ = 0;

  return *this;
}

}
}