#include <input/Cursor.hpp>

namespace monkeysworld {
namespace input {

Cursor::Cursor(GLFWwindow* window) {
  window_ = window;
  cursor_ = NULL;
  locked_ = false;
}

glm::dvec2 Cursor::GetCursorPosition() {
  glm::dvec2 cur;
  glm::ivec2 win;
  glfwGetCursorPos(window_, &cur.x, &cur.y);
  glfwGetFramebufferSize(window_, &win.x, &win.y);
  if (cur.x >= 0 && cur.y >= 0 && cur.x < win.x && cur.y < win.y) {
    return cur;
  }

  return glm::dvec2(-1);
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