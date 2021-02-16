#ifndef KEY_LISTENER_H_
#define KEY_LISTENER_H_

#include <cinttypes>
#include <memory>

namespace monkeysworld {
namespace input {

class WindowEventManager;

class KeyListener {
 public:
  // creates a completely unbounded key listener.
  KeyListener();
  KeyListener(WindowEventManager* mgr, uint64_t id);
  KeyListener(const KeyListener& other) = delete;
  KeyListener& operator=(const KeyListener& other) = delete;
  KeyListener(KeyListener&& other);
  KeyListener& operator=(KeyListener&& other);
  ~KeyListener();
  
  const uint64_t id;
 private:
  WindowEventManager* mgr_;
};

}
}

#endif