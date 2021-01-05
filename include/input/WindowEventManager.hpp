#ifndef WINDOW_EVENT_MANAGER_H_
#define WINDOW_EVENT_MANAGER_H_

#include <GLFW/glfw3.h>

#include <cinttypes>
#include <functional>
#include <unordered_map>

#include <shared_mutex>

#include <utils/IDGenerator.hpp>

namespace monkeysworld {
namespace engine {
class Context;
}
}

namespace monkeysworld {
namespace input {

/**
 *  Stores callbacks, and provides locking access to them.
 */ 
struct callback_info {
  std::unordered_map<uint64_t, std::function<void(int, int, int)>> callbacks;    // maps descriptors to functions
  std::shared_timed_mutex set_lock;                                              // lock associated with struct scope
};

/**
 *  Stores info on a single event.
 */ 
struct event_info {
  GLFWwindow* window;
  int key;
  int scancode;
  int action;
  int mods;
};

/**
 *  Integrates with GLFW to allow components to listen to input updates.
 *  Events are stored as they arrive, then processed in order when
 *  `ProcessWaitingEvents` is called. Most likely, this should be
 *  called once per frame, after `glfwPollEvents` is called.
 */ 
class WindowEventManager {
  friend class ::monkeysworld::engine::Context;
 public:

  /**
   * Creates a new window manager.
   */ 
  WindowEventManager(GLFWwindow* window);
  
  /**
   *  Handles incoming GLFW events.
   */ 
  void GenerateKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods);

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

  /**
   * Removes the key listener associated with the provided event descriptor.
   * @param event_id - the ID of the event we wish to remove.
   * @returns true if the event was removed successfully -- false otherwise.
   */ 
  bool RemoveKeyListener(uint64_t event_id);

 private:
  // maps keycodes to callback sets
  std::unordered_map<int, std::shared_ptr<callback_info>> callbacks_;

  // maps event descriptors to key codes
  std::unordered_map<uint64_t, int> callback_to_key_;

  // lock for class scope fields
  std::shared_timed_mutex callback_mutex_;

  // generates new events
  static utils::IDGenerator event_desc_generator_;

  // stores incoming events so that they can be processed at once.
  std::vector<event_info> event_queue_;

  // lock for event queue
  std::shared_timed_mutex event_mutex_;
 protected:

    /**
   *  Process all waiting events.
   */  
  void ProcessWaitingEvents();

  /**
   *  Protected ctor for testing purposes.
   *  Tests should subclass this object and then define a nop ctor which uses this.
   *  Please don't fuck with it
   */ 
  WindowEventManager() {};

};

} // namespace input
} // namespace monkeysworld

#endif  // WINDOW_EVENT_MANAGER_H_