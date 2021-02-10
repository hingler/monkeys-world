#include <input/KeyListener.hpp>
#include <input/WindowEventManager.hpp>

namespace monkeysworld {
namespace input {

KeyListener::KeyListener(WindowEventManager* mgr, uint64_t new_id) : id(new_id) {
  mgr_ = mgr;
}

KeyListener::~KeyListener() {
  mgr_->RemoveKeyListener(id);
}

}
}