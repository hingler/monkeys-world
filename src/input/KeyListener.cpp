#include <input/KeyListener.hpp>
#include <input/WindowEventManager.hpp>

namespace monkeysworld {
namespace input {

KeyListener::KeyListener() : id(0) {
  mgr_ = nullptr;
}

KeyListener::KeyListener(WindowEventManager* mgr, uint64_t new_id) : id(new_id) {
  mgr_ = mgr;
}

KeyListener::KeyListener(KeyListener&& other) : id(other.id) {
  uint64_t* other_id = const_cast<uint64_t*>(&other.id);
  *other_id = 0;
}

KeyListener& KeyListener::operator=(KeyListener&& other) {
  if (mgr_ && id != 0) {
    mgr_->RemoveKeyListener(*this);
  }

  uint64_t* this_id = const_cast<uint64_t*>(&id);
  uint64_t* other_id = const_cast<uint64_t*>(&other.id);
  *this_id = other.id;
  *other_id = 0;

  return *this;
}

KeyListener::~KeyListener() {
  if (mgr_ && id != 0) {
    mgr_->RemoveKeyListener(*this);
  }
}

}
}