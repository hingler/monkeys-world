#include <engine/RenderContext.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace monkeysworld {
namespace engine {

using critter::camera_info;
using shader::light::spotlight_info;
using critter::Camera;

camera_info RenderContext::GetActiveCamera() const {
  return cam_info_;
}

const std::vector<spotlight_info>& RenderContext::GetSpotlights() const {
  return spotlights_;
}

RenderPass RenderContext::GetRenderPass() const {
  return rp_;
}

void RenderContext::SetActiveCamera(std::shared_ptr<Camera> cam) {
  if (cam) {
    cam_info_ = cam->GetCameraInfo();
  } else {
    // create a default camera
    cam_info_.position = glm::vec3(0, 0, 0);
    // need screen res here -- how could we pass it in?
    // create a stub camera which is passed in by the engine only if the camera cannot be found
    glm::mat4 persp = glm::perspective(45.0f, 1.85f, 0.01f, 1000.0f);
    cam_info_.view_matrix = glm::mat4(1.0);
    cam_info_.persp_matrix = persp;
    cam_info_.vp_matrix = persp;
  }
}

// store ref to vector instead of storing the vector itself? we set it up in engine and don't touch it
void RenderContext::SetSpotlights(const std::vector<spotlight_info>& spotlights) {
  // this copy could incur a cost? idk
  spotlights_ = spotlights;
}

void RenderContext::SetRenderPass(RenderPass rp) {
  rp_ = rp;
}

}
}