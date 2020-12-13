#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>

namespace monkeysworld {
namespace critter {

class Camera {
 public:
  virtual glm::mat4 GetViewMatrix() = 0;

  virtual void SetFov(float deg) = 0;

  /**
   *  Marks this camera as active.
   */ 
  virtual void SetAsActive() = 0;

};

}
}

#endif