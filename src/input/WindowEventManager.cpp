#include <input/WindowEventManager.hpp>

#include <boost/log/trivial.hpp>
#include <GLFW/glfw3.h>


#include <shared_mutex>

namespace monkeysworld {
namespace input {

utils::IDGenerator WindowEventManager::event_desc_generator_;

WindowEventManager::WindowEventManager(GLFWwindow* window) {
  glfwSetWindowUserPointer(window, this);
  // lambdas are equiv. to fptrs if there are no captures
  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowEventManager* that =
      reinterpret_cast<WindowEventManager*>(glfwGetWindowUserPointer(window));
    that->ProcessKeyEvent(window, key, scancode, action, mods);
  });
}

void WindowEventManager::ProcessKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
  std::shared_lock<std::shared_timed_mutex>(callback_mutex_);
  auto callbacks = callbacks_.find(key);
  if (callbacks != callbacks_.end()) {
    std::shared_lock<std::shared_timed_mutex>(callbacks->second->set_lock);
    for (auto callback : callbacks->second->callbacks) {
      callback.second(key, action, mods);
    }
  }
}

uint64_t WindowEventManager::RegisterKeyListener(int key, std::function<void(int, int, int)> callback) {
  std::unique_lock<std::shared_timed_mutex>(callback_mutex_);
  auto callbacks = callbacks_.find(key);
  std::shared_ptr<callback_info> info_ptr;
  if (callbacks == callbacks_.end()) {
    info_ptr = std::make_shared<callback_info>();
    callbacks_.insert(std::make_pair(key, info_ptr));
  } else {
    info_ptr = callbacks->second;
  }

  uint64_t listener_id = event_desc_generator_.GetUniqueId();
  callback_to_key_.insert(std::make_pair(listener_id, key));
  std::unique_lock<std::shared_timed_mutex>(info_ptr->set_lock);
  info_ptr->callbacks.insert(std::make_pair(listener_id, callback));
  return listener_id;
}

bool WindowEventManager::RemoveKeyListener(uint64_t event_id) {
  std::unique_lock<std::shared_timed_mutex>(callback_mutex_);
  auto entry = callback_to_key_.find(event_id);
  if (entry == callback_to_key_.end()) {
    return false;
  }

  int key = entry->second;
  auto info_entry = callbacks_.find(key);
  if (info_entry == callbacks_.end()) {
    // assert?
    BOOST_LOG_TRIVIAL(error) << "Invalid state reached in WindowEventManager: info not found for callback";
    return false;
  }

  std::unique_lock<std::shared_timed_mutex>(info_entry->second->set_lock);
  info_entry->second->callbacks.erase(event_id);
  if (info_entry->second->callbacks.size() == 0) {
    BOOST_LOG_TRIVIAL(info) << "No more callbacks associated with key " << key << ". Erasing...";
    callbacks_.erase(key);
  }

  return true;
}

} // namespace input
} // namespace monkeysworld