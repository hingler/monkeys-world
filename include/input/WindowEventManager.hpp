#ifndef WINDOW_EVENT_MANAGER_H_
#define WINDOW_EVENT_MANAGER_H_

#include <GLFW/glfw3.h>

#include <cinttypes>
#include <functional>
#include <unordered_map>

#include <shared_mutex>

#include <utils/IDGenerator.hpp>

namespace monkeysworld {
namespace input {

/**
 *  Stores callbacks, and provides locking access to them.
 */ 
struct callback_info {
  std::unordered_map<uint64_t, std::function<void(int, int, int)>> callbacks;    // maps callback identifiers to their respective descriptors
  std::shared_timed_mutex set_lock;                                              // lock associated with all callbacks for this key
};

/**
 *  Integrates with GLFW to allow components to listen to input updates.
 */ 
class WindowEventManager {
 public:
  /**
   * Creates a new window manager.
   */ 
  WindowEventManager(GLFWwindow* window);
  
  /**
   *  Handles incoming GLFW events.
   */ 
  void ProcessKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

  /**
   * Register a listener which will be called when an event is generated for the desired key.
   * @param key - The GLFW integer associated with the desired key.
   * @param callback - The callback which will be invoked when the respective event is invoked.
   *                   Callbacks must return void, and must accept three parameters:
   *                     - int: the GLFW identifier for the key which generated the event (see below, and https://www.glfw.org/docs/3.3.2/group__keys.html)
   *                     - int: the GLFW action associated with the event (see https://www.glfw.org/docs/3.3.2/group__input.html)
   *                     - int: Any key modifiers (see https://www.glfw.org/docs/3.3.2/group__mods.html)
   * @returns a uniquely identifying "event descriptor" which can be used to modify the event later.
   */  
  uint64_t RegisterKeyListener(int key, std::function<void(int, int, int)> callback);

 private:
  // record of all callbacks stored, by the associated key.
  std::unordered_map<int, std::shared_ptr<callback_info>> callbacks_;

  // associates callbacks with their associated key
  std::unordered_map<uint64_t, int> callback_to_key_;

  // lock for callbacks_ and callback_to_key_
  std::shared_timed_mutex callback_mutex_;

  // for generating new event descriptors
  static utils::IDGenerator event_desc_generator_;

};

} // namespace input
} // namespace monkeysworld

#endif  // WINDOW_EVENT_MANAGER_H_