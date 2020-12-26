#ifndef LIGHT_VISITOR_H_
#define LIGHT_VISITOR_H_

#include <shader/light/LightTypes.hpp>
#include <shader/light/SpotLight.hpp>

#include <critter/Visitor.hpp>

#include <unordered_set>
#include <memory>

namespace monkeysworld {
namespace critter {
namespace visitor {

/**
 *  Passes through the object hierarchy, isolating any lights that it comes by.
 */ 
class LightVisitor : public critter::Visitor {
 public:
  LightVisitor();

  // noop
  void Visit(std::shared_ptr<Object> o) override;
  void Visit(std::shared_ptr<GameObject> o) override;
  void Visit(std::shared_ptr<GameCamera> o) override;

  /**
   *  Adds the discovered spotlight to the list of known spotlights.
   *  @param o - visited spotlight.
   */ 
  void Visit(std::shared_ptr<shader::light::SpotLight> o) override;
  
  /**
   *  Resets the state of the visitor.
   */ 
  void Clear();
  
  // returns a list of all spotlights visited.
  const std::vector<std::shared_ptr<shader::light::SpotLight>>& GetSpotLights();
 private:
  /**
   *  Visits all children of object.
   */ 
  void VisitChildren(std::shared_ptr<Object>);
  std::vector<std::shared_ptr<shader::light::SpotLight>> spotlights_;

};

}
}
}

#endif