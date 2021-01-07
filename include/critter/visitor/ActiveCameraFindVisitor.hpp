#ifndef ACTIVE_CAMERA_FIND_VISITOR_H_
#define ACTIVE_CAMERA_FIND_VISITOR_H_

// raise a flag once we've found an active camera
// check said flag for each child

#include <critter/Visitor.hpp>

#include <shader/light/SpotLight.hpp>
#include <critter/GameObject.hpp>

#include <atomic>

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
  void Visit(std::shared_ptr<font::TextObject> o) override;

  /**
   *  Returns the active camera, or nullptr if it cannot be found.
   */ 
  std::shared_ptr<GameCamera> GetActiveCamera();

  /**
   *  Ensure that the state of the visitor is reset.
   */ 
  void Clear();
 private:
  void ActiveCameraVisitChildren(std::vector<std::weak_ptr<Object>>&);

  std::shared_ptr<GameCamera> active_camera_;
  std::atomic_bool cam_found_;
};

}
}
}

#endif