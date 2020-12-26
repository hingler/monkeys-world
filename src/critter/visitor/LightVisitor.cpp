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

void LightVisitor::Visit(std::shared_ptr<SpotLight> o) {
  // add to the spotlight list
  // i already know it's a spotlight, so this should be fine
  spotlights_.push_back(o);
}

void LightVisitor::Clear() {
  spotlights_.clear();
}

}
}
}