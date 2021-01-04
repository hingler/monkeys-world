#include <critter/visitor/LightVisitor.hpp>

namespace monkeysworld {
namespace critter {
namespace visitor {

typedef std::vector<std::shared_ptr<shader::light::SpotLight>> spotlight_list;

using shader::light::SpotLight;

LightVisitor::LightVisitor() {}

const spotlight_list& LightVisitor::GetSpotLights() {
  return spotlights_; 
}

void LightVisitor::Visit(std::shared_ptr<Object> o) {
  VisitChildren(o);
}

void LightVisitor::Visit(std::shared_ptr<GameObject> o) {
  VisitChildren(std::static_pointer_cast<Object>(o));
}

void LightVisitor::Visit(std::shared_ptr<GameCamera> o) {
  VisitChildren(std::static_pointer_cast<Object>(o));
}



void LightVisitor::Visit(std::shared_ptr<SpotLight> o) {
  // add to the spotlight list
  // i already know it's a spotlight, so this should be fine
  spotlights_.push_back(o);
  VisitChildren(std::static_pointer_cast<Object>(o));
}

void LightVisitor::VisitChildren(std::shared_ptr<Object> o) {
  for (auto child : o->GetChildren()) {
    auto shared_child = child.lock();
    if (shared_child != nullptr) {
      // TODO: Accept should probably accept ptr vs reference
      shared_child->Accept(*this);
    }
  }
}

void LightVisitor::Clear() {
  spotlights_.clear();
}

}
}
}