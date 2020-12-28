#ifndef ACTIVE_CAMERA_FIND_VISITOR_H_
#define ACTIVE_CAMERA_FIND_VISITOR_H_

// raise a flag once we've found an active camera
// check said flag for each child

#include <critter/Visitor.hpp>

#include <shader/light/SpotLight.hpp>
#include <critter/GameObject.hpp>

namespace monkeysworld {
namespace critter {
namespace visitor {

/**
 *  Visitor which finds the active camera in the component hierarchy.
 */ 
class ActiveCameraFindVisitor : public critter::Visitor {
 public:
  /**
   *  Initialize the visitor.
   */
  ActiveCameraFindVisitor();

  void Visit(std::shared_ptr<Object> o) override;
  void Visit(std::shared_ptr<GameObject> o) override;
  void Visit(std::shared_ptr<GameCamera> o) override;
  void Visit(std::shared_ptr<shader::light::SpotLight> o) override;

  /**
   *  Returns the active camera, or nullptr if it cannot be found.
   */ 
  std::shared_ptr<GameCamera> GetActiveCamera();
 private:
  std::shared_ptr<GameCamera> active_camera_;
};

}
}
}

#endif