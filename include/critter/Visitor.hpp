#ifndef VISITOR_H_
#define VISITOR_H_

#include <critter/Object.hpp>
#include <critter/GameObject.hpp>
#include <critter/Model.hpp>
#include <critter/GameCamera.hpp>
#include <font/TextObject.hpp>

#include <shader/light/SpotLight.hpp>

#include <memory>

namespace monkeysworld {
namespace critter {

/**
 *  Visitor dispatched to element hierarchy
 * 
 *  One pipeline -- how do we customize?
 *  Probably...
 *  TODO: Add a tagging system for components
 *  at Object level so that we can handle objects
 *  uniquely
 * 
 *  we still want a pass system though
 */ 
class Visitor {
 public:
  // object default
  virtual void Visit(std::shared_ptr<Object> o) = 0;
  // game object implementors
  virtual void Visit(std::shared_ptr<GameObject> o) = 0;
  // and other special object types
  virtual void Visit(std::shared_ptr<GameCamera> o) = 0;
  virtual void Visit(std::shared_ptr<shader::light::SpotLight> o) = 0;
  virtual void Visit(std::shared_ptr<font::TextObject> o) = 0;
 protected:
  /** ### ASSISTING FUNCTIONS ### **/
  void VisitChildren(std::shared_ptr<Object> o);
};

} // namespace render
} // namespace monkeysworld

#endif  // RENDERER_H_