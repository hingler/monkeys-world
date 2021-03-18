#ifndef GAME_CAMERA_H_
#define GAME_CAMERA_H_

#include <critter/Camera.hpp>
#include <critter/GameObject.hpp>
#include <glm/glm.hpp>

#include <engine/RenderContext.hpp>

namespace monkeysworld {
namespace critter {
  /**
   *  Represents the camera within a scene.
   *  TODO: Work this into the model test to ensure that it functions properly
   */ 
class GameCamera : public GameObject, public Camera {
 public:
  /**
   *  Constructs a new camera.
   */ 
  GameCamera(engine::Context* ctx);

  void Accept(Visitor& v) override;

  // lazy noops
  void RenderMaterial(const engine::RenderContext& rc) override {}
  void PrepareAttributes() override {}
  void Draw() override {}

  /**
   *  Sets the current active state of this camera.
   *  If the camera is inactive, does nothing.
   */ 
  void SetActive(bool isActive) override;

  /**
   *  Returns the view+proj matrix associated with this camera.
   */ 
  glm::mat4 GetViewMatrix() const override;

  /**
   *  Impl for GetCameraInfo
   */ 
  camera_info GetCameraInfo() const override;

  /**
   *  Sets the field of view of the camera.
   *  @param deg - the new FOV, in degrees.
   */ 
  void SetFov(float deg) override;

  glm::vec2 ToScreenCoords(glm::vec3 input) const override;

  bool IsActive();
 private:
  float fov_deg_;
  bool active_;
};
}
}

#endif