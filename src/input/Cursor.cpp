#include <input/Cursor.hpp>

namespace monkeysworld {
namespace input {

Cursor::Cursor(GLFWwindow* window) {
  window_ = window;
  cursor_ = NULL;
  locked_ = false;
}

glm::dvec2 Cursor::GetCursorPosition() {
  glm::dvec2 cursor_pos;
  glfwGetCursorPos(window_, &cursor_pos.x, &cursor_pos.y);
  return cursor_pos;
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