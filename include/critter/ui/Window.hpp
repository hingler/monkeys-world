#ifndef WINDOW_H_
#define WINDOW_H_

#include <cinttypes>
#include <memory>
#include <vector>

#include <critter/ui/UIObject.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {

/**
 *  The window class is the root of all UI objects.
 *  Scene objects each contain their own window, which contains all UIObjects
 *  which the user intends to display on the screen.
 * 
 *  The window always occupies the full width of the screen.
 */ 
class Window {
 public:
  const static uint64_t ID = 1;
  
  uint64_t GetId() {
    return ID;
  }

  // these functions behave the same way that they would for a UIObject.
  virtual void AddChild(std::shared_ptr<UIObject> obj) = 0;
  virtual void RemoveChild(uint64_t id) = 0;
  virtual std::shared_ptr<Object> GetChild(uint64_t id) = 0;
  virtual std::vector<std::shared_ptr<Object>> GetChildren() = 0;
};

}
}
}

#endif