#include <input/Cursor.hpp>

namespace monkeysworld {
namespace input {

Cursor::Cursor(GLFWwindow* window) {
  window_ = window;
  cursor_ = NULL;
  locked_ = false;
}

glm::dvec2 Cursor::GetCursorPosition() {
  return cursor_cache_;
}

void Cursor::UpdateCursorPosition() {
  glm::ivec2 win;
  glfwGetCursorPos(window_, &cursor_cache_.x, &cursor_cache_.y);
  glfwGetFramebufferSize(window_, &win.x, &win.y);
}

void Cursor::LockCursor() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  locked_ = true;
}

void Cursor::UnlockCursor() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  locked_ = false;
}

bool Cursor::IsCursorLocked() {
  return locked_;
}

Cursor::~Cursor() {
  // nothing yet but there might be! soon! :)
}

}
}