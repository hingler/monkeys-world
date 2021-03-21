#include <input/WindowEventManager.hpp>

#include <input/ClickListener.hpp>
#include <input/KeyListener.hpp>

#include <boost/log/trivial.hpp>

#include <engine/Context.hpp>
#include <engine/Scene.hpp>

#include <shared_mutex>
#include <mutex>

namespace monkeysworld {
namespace input {

utils::IDGenerator WindowEventManager::event_desc_generator_;

WindowEventManager::WindowEventManager(GLFWwindow* window, engine::Context* ctx) {
  glfwSetWindowUserPointer(window, this);
  // lambdas are equiv. to fptrs if there are no captures
  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowEventManager* that =
      reinterpret_cast<WindowEventManager*>(glfwGetWindowUserPointer(window));
    that->GenerateKeyEvent(window, key, scancode, action, mods);
  });

  glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
    WindowEventManager* that =
      reinterpret_cast<WindowEventManager*>(glfwGetWindowUserPointer(window));
    that->GenerateClickEvent(window, button, action, mods);
  });

  // ctx is tied here -- does not update
  ctx_ = ctx;
  window_ = window;

  cursor_ = std::make_shared<Cursor>(window);
}

std::shared_ptr<Cursor> WindowEventManager::GetCursor() {
  return cursor_;
}

void WindowEventManager::GenerateKeyEvent(GLFWwindow* window, int key, int scancode, int action, int mods) {
  std::unique_lock<std::shared_timed_mutex>(event_mutex_);
  event_queue_.push_back({window, key, scancode, action, mods});
}

void WindowEventManager::GenerateClickEvent(GLFWwindow* window, int button, int action, int mods) {
  std::unique_lock<std::shared_timed_mutex>(event_mutex_);
  event_queue_.push_back({window, button, -1, action, mods});
}

void WindowEventManager::ProcessWaitingEvents() {
  // TODO: this call fails in tests, since window is not defined
  cursor_->UpdateCursorPosition();
  std::shared_lock<std::shared_timed_mutex>(callback_mutex_);
  std::vector<event_info> events;
  {
    std::unique_lock<std::shared_timed_mutex>(event_mutex_);
    events = event_queue_;
    event_queue_.clear();
  }

  for (auto event : events) {
    if (event.scancode < 0) {
      // mouse event
      double x_pos;
      double y_pos;
      MouseEvent e;
      // prepare mouse event
      glfwGetCursorPos(window_, &x_pos, &y_pos);
      e.absolute_pos.x = static_cast<float>(x_pos);
      e.absolute_pos.y = static_cast<float>(y_pos);
      e.button = event.key;
      e.action = event.action;
      e.mods = event.mods;

      for (auto callback : mouse_callbacks_) {
        callback.second(e);
      }

      e.local_pos = e.absolute_pos;
      // tba: is there a better way to update this?
      auto window = ctx_->GetScene()->GetWindow();
      auto ui_dims = window->GetDimensions();
      // if this was guaranteed to be the window, we could save some time
      if (e.local_pos.x >= 0 && e.local_pos.y >= 0
         && e.local_pos.x < ui_dims.x && e.local_pos.y < ui_dims.y
         && !cursor_->IsCursorLocked()) {
           // grab old window
           window->HandleClickEvent(e);
      }
    } else {
      // key event
      auto callbacks = callbacks_.find(event.key);
      if (callbacks != callbacks_.end()) {
        // store ptr so that all methods run before dealloc
        auto ptr = callbacks->second;
        std::lock<std::recursive_mutex, std::recursive_mutex>(ptr->set_lock, del_mutex_);
        std::lock_guard<std::recursive_mutex> l1(ptr->set_lock, std::adopt_lock);
        std::lock_guard<std::recursive_mutex> l2(del_mutex_, std::adopt_lock);
        for (auto callback : ptr->callbacks) {
          if (callback_delete_queue_.find(callback.first) == callback_delete_queue_.end()) {
            callback.second(event.key, event.action, event.mods);
          }
        }

      }
    }
  }

  // run through and do any deletions we might need
  FlushDeleteQueue();
}

void WindowEventManager::FlushDeleteQueue() {
  std::unique_lock<std::recursive_mutex> lock_queue(del_mutex_);
  std::unique_lock<std::shared_timed_mutex> lock_callback(callback_mutex_);
  // one pass for keys...

  {
    int key;
    auto itr = callback_delete_queue_.begin();
    while (itr != callback_delete_queue_.end()) {
      BOOST_LOG_TRIVIAL(trace) << "removing event " << *itr;
      auto entry = callback_to_key_.find(*itr);
      if (entry == callback_to_key_.end()) {
        itr++;
        continue;
      }
      
      key = entry->second;
      auto info_entry = callbacks_.find(key);
      if (info_entry == callbacks_.end()) {
        BOOST_LOG_TRIVIAL(error) << "BAD CALLBACK STATE!";
      }

      auto callback_info = info_entry->second;
      std::unique_lock<std::recursive_mutex> callback_lock(callback_info->set_lock);
      callback_info->callbacks.erase(*itr);
      if (callback_info->callbacks.size() == 0) {
        BOOST_LOG_TRIVIAL(info) << "No more callbacks associated with key " << key << ". Erasing...";
        callbacks_.erase(key);
      }

      itr = callback_delete_queue_.erase(itr);
    }
  }

  // one pass for clicks...
  {
    auto itr = callback_delete_queue_.begin();
    while (itr != callback_delete_queue_.end()) {
      auto entry = mouse_callbacks_.find(*itr);
      if (entry != mouse_callbacks_.end()) {
        mouse_callbacks_.erase(entry);
      }

      itr = callback_delete_queue_.erase(itr);
    }
  }
}

uint64_t WindowEventManager::RegisterKeyListener(int key, std::function<void(int, int, int)> callback) {
  std::unique_lock<std::shared_timed_mutex>(callback_mutex_);
  auto callbacks = callbacks_.find(key);
  std::shared_ptr<key_callback_info> info_ptr;
  if (callbacks == callbacks_.end()) {
    info_ptr = std::make_shared<key_callback_info>();
    callbacks_.insert(std::make_pair(key, info_ptr));
  } else {
    info_ptr = callbacks->second;
  }

  uint64_t listener_id = event_desc_generator_.GetUniqueId();
  callback_to_key_.insert(std::make_pair(listener_id, key));
  {
    std::lock_guard<std::recursive_mutex>(info_ptr->set_lock);
    info_ptr->callbacks.insert(std::make_pair(listener_id, callback));
  }
  return listener_id;
}

KeyListener WindowEventManager::CreateKeyListener(int key, std::function<void(int, int, int)> callback) {
  uint64_t id = RegisterKeyListener(key, callback);
  BOOST_LOG_TRIVIAL(trace) << "new listener: " << id;
  return KeyListener(this, id);
}

uint64_t WindowEventManager::RegisterClickListener(std::function<void(MouseEvent)> callback) {
  // use callback mutex to add to mouse_callbacks.
  std::unique_lock<std::shared_timed_mutex>(callback_mutex_);
  uint64_t listener_id = event_desc_generator_.GetUniqueId();
  mouse_callbacks_.insert(std::make_pair(listener_id, callback));
  return listener_id;
}

ClickListener WindowEventManager::CreateClickListener(std::function<void(MouseEvent)> callback) {
  uint64_t id = RegisterClickListener(callback);
  return ClickListener(this, id);
}

bool WindowEventManager::RemoveKeyListener(uint64_t event_id) {
  std::lock_guard<std::recursive_mutex> lock(del_mutex_);
  callback_delete_queue_.insert(event_id);
  return true;
}


void WindowEventManager::RemoveKeyListener(KeyListener& k) {
  RemoveKeyListener(k.id);
}

void WindowEventManager::RemoveClickListener(ClickListener& c) {
  RemoveClickListener(c.id);
}

bool WindowEventManager::RemoveClickListener(uint64_t event_id) {
  std::lock_guard<std::recursive_mutex> lock(del_mutex_);
  callback_delete_queue_.insert(event_id);
  return true;
}

} // namespace input
} // namespace monkeysworld