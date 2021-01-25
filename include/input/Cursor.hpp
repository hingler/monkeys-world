#ifndef CURSOR_H_
#define CURSOR_H_

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace monkeysworld {
namespace input {

/**
 *  The cursor object provides clients with access to functionality
 *  relating specifically to the cursor.
 * 
 *  This includes modifying the cursor image, fetching the cursor's position and click state,
 *  and locking/unlocking the cursor.
 */ 
class Cursor {
 public:
  /**
   *  Creates a new cursor object.
   *  @param window - the window this cursor is associated with.
   */ 
  Cursor(GLFWwindow* window);

  /**
   *  @returns the position of the cursor relative to the top left corner of the screen.
   */ 
  glm::dvec2 GetCursorPosition();

  /**
   *  Returns whether or not a mouse button is pressed.
   *  @param key - the GLFW key identifier associated with the desired key press.
   */ 
  bool IsMouseButtonPressed(int key);

  /**
   *  Locks the mouse to the window, allowing for "unlimited" mouse movement.
   */ 
  void LockCursor();

  /**
   *  Unlocks the cursor.
   */ 
  void UnlockCursor();

  /**
   *  @returns true if the cursor is locked, and false otherwise.
   */ 
  bool IsCursorLocked();

  ~Cursor();
 private:
  GLFWwindow* window_;
  GLFWcursor* cursor_;
  bool locked_;
};

}
}

#endif