#ifndef KEY_LISTENER_H_
#define KEY_LISTENER_H_

#include <cinttypes>
#include <memory>

namespace monkeysworld {
namespace input {

class WindowEventManager;

class KeyListener {
 public:
  KeyListener(WindowEventManager* mgr, uint64_t id);
  ~KeyListener();
  const uint64_t id;
 private:
  WindowEventManager* mgr_;
};

}
}

#endif