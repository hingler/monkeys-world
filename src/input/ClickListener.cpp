#include <input/ClickListener.hpp>
#include <input/WindowEventManager.hpp>

namespace monkeysworld {
namespace input {

// TODO: ClickListener needs to implement move/copy, because otherwise we'll lose these.

ClickListener::ClickListener(WindowEventManager* mgr, uint64_t new_id) : id(new_id) {
  mgr_ = mgr;
}

ClickListener::~ClickListener() {
  mgr_->RemoveClickListener(id);
}

}
}