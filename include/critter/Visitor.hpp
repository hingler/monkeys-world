#ifndef VISITOR_H_
#define VISITOR_H_

#include <critter/Object.hpp>
#include <critter/GameObject.hpp>
#include <critter/Model.hpp>
#include <critter/GameCamera.hpp>

#include <shader/light/SpotLight.hpp>

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
  virtual void Visit(Object* o) = 0;
  // game object implementors
  virtual void Visit(GameObject* o) = 0;
  // and other special object types
  virtual void Visit(GameCamera* o) = 0;
  virtual void Visit(shader::light::SpotLight* o) = 0;
};

} // namespace render
} // namespace monkeysworld

#endif  // RENDERER_H_