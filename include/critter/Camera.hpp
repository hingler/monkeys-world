#ifndef CAMERA_H_
#define CAMERA_H_

#include <glm/glm.hpp>

namespace monkeysworld {
namespace critter {

/**
 *  Shader representation of camera.
 */ 
struct camera_info {
  glm::vec3 position;       // position of the camera
  glm::mat4 vp_matrix;      // view matrix and perspective matrix combined
  glm::mat4 view_matrix;    // view matrix only
  glm::mat4 persp_matrix;   // perspective matrix only
};

/**
 *  Representation of a camera in space.
 */ 
class Camera {
 public:
  virtual glm::mat4 GetViewMatrix() const = 0;

  virtual void SetFov(float deg) = 0;

  /**
   *  Marks this camera as active.
   */ 
  virtual void SetActive(bool isActive) = 0;

  /**
   *  Returns a new camera info object, associated with this camera.
   */ 
  virtual camera_info GetCameraInfo() const = 0;

  /**
   *  Converts a 3d coordinate to one in screen space, relative to this camera.
   *  @param input - the 3d coordinate input.
   *  @returns the pixel position of that 3d point, relative to top left corner.
   */ 
  virtual glm::vec2 ToScreenCoords(glm::vec3 input) const = 0;

};

}
}

#endif