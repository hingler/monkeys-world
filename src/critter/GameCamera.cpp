#include <critter/GameCamera.hpp>
#include <critter/Visitor.hpp>
#include <boost/log/trivial.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace monkeysworld {
namespace critter {

using engine::Context;

GameCamera::GameCamera(Context* ctx) : GameObject(ctx) {
  fov_deg_ = 45.0f;
  active_ = false;
}

void GameCamera::Accept(Visitor& v) {
  v.Visit(std::dynamic_pointer_cast<GameCamera>(this->shared_from_this()));
}

glm::mat4 GameCamera::GetViewMatrix() const {
  // this should work :)
  int width, height;
  // we could inverse scale this
  GetContext()->GetFramebufferSize(&width, &height);
  glm::mat4 persp = glm::perspective(glm::radians(fov_deg_), (float)width / height, 0.01f, 100.0f);
  // view tf should be inverse of model tf
  // cache this op whenever we can!
  glm::mat4 vm = GetTransformationMatrix();
  // undo scaling effects
  return persp * glm::inverse(vm);
}

void GameCamera::SetActive(bool isActive) {
  this->active_ = isActive;

  // TODO: when first creating the scene,
  //       assume the camera with the lowest ID is active.
}

void GameCamera::SetFov(float deg) {
  fov_deg_ = deg;
}

camera_info GameCamera::GetCameraInfo() const {
  camera_info res;
  res.position = GetPosition();
  res.view_matrix = GetViewMatrix();
  return res;
}

bool GameCamera::IsActive() {
  return active_;
}

}
}