#ifndef SCENE_H_
#define SCENE_H_

#include <critter/Object.hpp>

namespace monkeysworld {
namespace engine {

/**
 *  A scene represents a series of deployed assets.
 *  For the time being, scenes expose root-level assets,
 *  allowing the engine to pass functions to these assets recursively.
 * 
 *  Implementors should pass in context on ctor.
 * 
 *  (TBA)
 *  A scene should be able to define constants which are accessible elsewhere.
 *  For instance: IDs.
 *  TODO: Add a "SetID" method to our id generator
 */ 
class Scene {
 public:

  /**
   *  Used by the scene to set up all objects.
   *  TODO: how could we create a new scene?
   * 
   *  Barebones soln: use an enum to communicate to the engine which scene we should pull up.
   *  The context only has to know the enum value, which the engine will have to interpret.
   *  It's not perfect, but then again, who else is going to use this PoS?
   * 
   *  I'll be the client, so I'll have to write some function which can accept the enum
   *  and return a scene.
   */ 
  virtual void Initialize() = 0;

  /**
   *  Our engine only really needs to know that it's an object,
   *  and our components will know better. So this should be OK.
   */ 
  virtual std::shared_ptr<critter::Object> GetGameObjectRoot() = 0;
};

}
}

#endif