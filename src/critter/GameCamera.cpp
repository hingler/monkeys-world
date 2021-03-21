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

// Problem: skybox requires view separate from perspective, so that we can separate one from the other
// soln: add a method which returns the view and perspective matrices separately

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
}

void GameCamera::SetFov(float deg) {
  fov_deg_ = deg;
}

camera_info GameCamera::GetCameraInfo() const {
  camera_info res;
  res.position = GetPosition();
  res.vp_matrix = GetViewMatrix();
  res.view_matrix = glm::inverse(GetTransformationMatrix());
  int width, height;
  GetContext()->GetFramebufferSize(&width, &height);
  res.persp_matrix = glm::perspective(glm::radians(fov_deg_), (float)width / height, 0.01f, 100.0f);

  return res;
}

glm::vec2 GameCamera::ToScreenCoords(glm::vec3 input) const {
  camera_info info = GetCameraInfo();
  glm::vec4 output = info.vp_matrix * glm::vec4(input, 1.0);
  output = output / output.w;
  glm::vec2 screen_dims = GetContext()->GetFramebufferSize();
  output = (output + 1.0f) / 2.0f;
  output.y = 1.0f - output.y;
  return glm::vec2(output.x, output.y) * screen_dims;
}

bool GameCamera::IsActive() {
  return active_;
}

}
}