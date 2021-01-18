#include <engine/RenderContext.hpp>

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
  cam_info_ = cam->GetCameraInfo();
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