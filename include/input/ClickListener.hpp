#ifndef CLICK_LISTENER_H_
#define CLICK_LISTENER_H_

#include <cinttypes>
#include <memory>

namespace monkeysworld {
namespace input {

class WindowEventManager;

// raii click listener class
class ClickListener {
 public:
  /**
   *  Creates a new click listener.
   *  @param mgr - reference to the manager.
   *  @param id - the new id associated with this click listener.
   */ 
  ClickListener(WindowEventManager* mgr, uint64_t id);
  ~ClickListener();
  const uint64_t id;
 private:
  WindowEventManager* mgr_;
};

}
}

#endif