#include <critter/GameCamera.hpp>
#include <critter/Visitor.hpp>
#include <boost/log/trivial.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

namespace monkeysworld {
namespace critter {

GameCamera::GameCamera(Context* ctx) : GameObject(ctx) {
  fov_deg_ = 45.0f;
  active_ = false;
}

void GameCamera::Accept(Visitor& v) {
  v.Visit(this);
}

glm::mat4 GameCamera::GetViewMatrix() {
  // this should work :)
  int width, height;
  // we could inverse scale this
  GetContext()->GetFramebufferSize(&width, &height);
  glm::mat4 persp = glm::perspective(glm::radians(fov_deg_), (float)width / height, 0.01f, 100.0f);
  // view tf should be inverse of model tf
  // cache this op whenever we can!
  glm::mat4 vm = GetTransformationMatrix();
  // undo scaling effects
  glm::vec3 inverse_scale = GetScale();
  return persp * glm::inverse(vm);
}

void GameCamera::SetFov(float deg) {
  fov_deg_ = deg;
}

}
}