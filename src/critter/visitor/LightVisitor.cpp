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

void LightVisitor::Visit(SpotLight* o) {
  // add to the spotlight list
  // i already know it's a spotlight, so this should be fine
  spotlights_.push_back(std::reinterpret_pointer_cast<SpotLight>(o->shared_from_this()));
}

void LightVisitor::Clear() {
  spotlights_.clear();
}

}
}
}