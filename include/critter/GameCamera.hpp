#ifndef GAME_CAMERA_H_
#define GAME_CAMERA_H_

#include <critter/GameObject.hpp>
#include <critter/Camera.hpp>
#include <glm/glm.hpp>

namespace monkeysworld {
namespace critter {
  /**
   *  Represents the camera within a scene.
   *  TODO: Work this into the model test to ensure that it functions properly
   */ 
class GameCamera : public Camera, public GameObject {
 public:
  /**
   *  Constructs a new camera.
   */ 
  GameCamera(Context* ctx);

  void RenderMaterial() override {}
  void PrepareAttributes() override {}
  void SetAsActive() override {}

  /**
   *  Returns the view+proj matrix associated with this camera.
   */ 
  glm::mat4 GetViewMatrix() override;

  /**
   *  Sets the field of view of the camera.
   */ 
  void SetFov(float deg) override;
 private:
  float fov_deg_;
  bool active_;
};
}
}

#endif