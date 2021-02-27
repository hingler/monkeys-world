#include <input/ClickListener.hpp>
#include <input/WindowEventManager.hpp>

namespace monkeysworld {
namespace input {

ClickListener::ClickListener(WindowEventManager* mgr, uint64_t new_id) : id(new_id) {
  mgr_ = mgr;
}

ClickListener::~ClickListener() {
  mgr_->RemoveClickListener(*this);
}

}
}