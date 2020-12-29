#include <critter/visitor/ActiveCameraFindVisitor.hpp>

namespace monkeysworld {
namespace critter {
namespace visitor {

ActiveCameraFindVisitor::ActiveCameraFindVisitor() : active_camera_(nullptr), cam_found_(false) {}

void ActiveCameraFindVisitor::Visit(std::shared_ptr<Object> o) {
  auto children = o->GetChildren();
  ActiveCameraVisitChildren(children);

}

void ActiveCameraFindVisitor::Visit(std::shared_ptr<GameObject> o) {
  auto children = o->GetChildren();
  ActiveCameraVisitChildren(children);
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
  auto children = o->GetChildren();
  ActiveCameraVisitChildren(children);
}

void ActiveCameraFindVisitor::ActiveCameraVisitChildren(std::vector<std::weak_ptr<Object>>& ol) {
  // visit all children
  // check if flag is raised
  // if so: break
  for (auto child : ol) {
    auto shared_child = child.lock();
    if (shared_child) {
      shared_child->Accept(*this);
    }

    if (cam_found_.load()) {
      break;
    }
  }
}

}
}
}