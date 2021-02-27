#include <critter/visitor/ActiveCameraFindVisitor.hpp>

namespace monkeysworld {
namespace critter {
namespace visitor {

ActiveCameraFindVisitor::ActiveCameraFindVisitor() : active_camera_(nullptr), cam_found_(false) {}

void ActiveCameraFindVisitor::Visit(std::shared_ptr<Object> o) {
  if (!cam_found_.load()) {
    auto children = o->GetChildren();
    ActiveCameraVisitChildren(children);
  }
}

void ActiveCameraFindVisitor::Visit(std::shared_ptr<GameObject> o) {
  if (!cam_found_.load()) {
    auto children = o->GetChildren();
    ActiveCameraVisitChildren(children);
  }
}

void ActiveCameraFindVisitor::Visit(std::shared_ptr<GameCamera> o) {
  // check if the camera is active
  // if so: set as active and return.
  if (o->IsActive()) {
    active_camera_ = o;
    cam_found_.store(true);
  } else {
    auto children = o->GetChildren();
    ActiveCameraVisitChildren(children);
  }
}

void ActiveCameraFindVisitor::Visit(std::shared_ptr<shader::light::SpotLight> o) {
  if (!cam_found_.load()) {
    auto children = o->GetChildren();
    ActiveCameraVisitChildren(children);
  }
}

void ActiveCameraFindVisitor::Visit(std::shared_ptr<font::TextObject> o) {
  if (!cam_found_.load()) {
    auto children = o->GetChildren();
    ActiveCameraVisitChildren(children);
  }
}

std::shared_ptr<GameCamera> ActiveCameraFindVisitor::GetActiveCamera() {
  if (cam_found_) {
    return active_camera_;
  }

  return std::shared_ptr<GameCamera>();
}

void ActiveCameraFindVisitor::Clear() {
  active_camera_ = std::shared_ptr<GameCamera>();
  cam_found_.store(false);
}

void ActiveCameraFindVisitor::ActiveCameraVisitChildren(std::vector<std::shared_ptr<Object>>& ol) {
  // visit all children
  // check if flag is raised
  // if so: break
  for (auto child : ol) {
    child->Accept(*this);

    if (cam_found_.load()) {
      break;
    }
  }
}

}
}
}