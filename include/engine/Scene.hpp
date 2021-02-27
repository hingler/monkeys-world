#ifndef SCENE_H_
#define SCENE_H_

#include <critter/Object.hpp>
#include <critter/GameObject.hpp>
#include <critter/ui/UIObject.hpp>

#include <engine/EngineWindow.hpp>

namespace monkeysworld {
namespace engine {

class Context;

/**
 *  A scene represents a series of deployed assets.
 *  For the time being, scenes expose root-level assets,
 *  allowing the engine to pass functions to these assets recursively.
 * 
 *  Implementors should pass in context on ctor.
 *  Note too that the context should never change, so this should be ok. We just need our
 *  new objects to have a reference to it.
 */ 
class Scene {
 public:

  Scene();

  /**
   *  Called to set up scenes.
   *  Calls Initialize, passing the context to it.
   */ 
  void CreateScene(Context* ctx);

  /**
   *  Used by the scene to set up all objects.
   */ 
  virtual void Initialize(Context* ctx) = 0;

  /**
   *  This method must be overwritten for each scene.
   *  Returns the name of this scene.
   */ 
  virtual std::string GetSceneIdentifier() = 0;

  /**
   *  Our engine only really needs to know that it's an object,
   *  and our components will know better. So this should be OK.
   */ 
  std::shared_ptr<critter::GameObject> GetGameObjectRoot();
  std::shared_ptr<critter::ui::Window> GetWindow();
 private:
  std::shared_ptr<critter::GameObject> game_root_;
  std::shared_ptr<EngineWindow> ui_window_;
};

}
}

#endif