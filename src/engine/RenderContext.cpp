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

void RenderContext::SetActiveCamera(std::shared_ptr<Camera> cam) {
  cam_info_ = cam->GetCameraInfo();
}

void RenderContext::SetSpotlights(const std::vector<spotlight_info>& spotlights) {
  // this copy could incur a cost? idk
  spotlights_ = spotlights;
}

}
}