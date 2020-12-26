#ifndef GAME_CAMERA_H_
#define GAME_CAMERA_H_

#include <critter/GameObject.hpp>
#include <glm/glm.hpp>

namespace monkeysworld {
namespace critter {
  /**
   *  Represents the camera within a scene.
   *  TODO: Work this into the model test to ensure that it functions properly
   */ 
class GameCamera : public GameObject {
 public:
  /**
   *  Constructs a new camera.
   */ 
  GameCamera(Context* ctx);

  void Accept(Visitor& v) override;

  // lazy noops
  void RenderMaterial() override {}
  void PrepareAttributes() override {}
  void Draw() override {}

  /**
   *  Sets the current active state of this camera.
   *  If the camera is inactive, does nothing.
   */ 
  void SetActive(bool isActive);

  /**
   *  Returns the view+proj matrix associated with this camera.
   */ 
  glm::mat4 GetViewMatrix();

  /**
   *  Sets the field of view of the camera.
   *  @param deg - the new FOV, in degrees.
   */ 
  void SetFov(float deg);

  bool IsActive();
 private:
  float fov_deg_;
  bool active_;
};
}
}

#endif