#ifndef LIGHT_VISITOR_H_
#define LIGHT_VISITOR_H_

#include <shader/light/LightTypes.hpp>
#include <shader/light/SpotLight.hpp>

#include <critter/Visitor.hpp>

#include <memory>

namespace monkeysworld {
namespace critter {
namespace visitor {

class LightVisitor : public critter::Visitor {
 public:
  LightVisitor();

  // noop
  virtual void Visit(Object* o) override;
  virtual void Visit(GameObject* o) override;
  virtual void Visit(GameCamera* o) override;

  /**
   *  Adds the discovered spotlight to the list of known spotlights.
   */ 
  virtual void Visit(shader::light::SpotLight* o) override;
  
  /**
   *  Resets the state of the visitor.
   */ 
  void Clear();
  
  // returns a list of all spotlights visited.
  const std::vector<const std::shared_ptr<shader::light::SpotLight>>& GetSpotLights();
  
}

}
}
}

#endif