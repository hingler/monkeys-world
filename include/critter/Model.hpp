#ifndef MODEL_H_
#define MODEL_H_

#include <critter/GameObject.hpp>
#include <critter/Context.hpp>

namespace monkeysworld {
namespace critter {

/**
 *  A type of GameObject which contains visible geometry.
 */ 
class Model : public GameObject {
 public:

  /**
   *  Creates a new Model
   */ 
  Model(Context* ctx);
  /**
   *  Binds VAO associated with vertices.
   */ 
  void PrepareAttributes() override;

  /**
   *  Draws the object onto the screen.
   */ 
  void RenderMaterial() override;
};

} // namespace critter
} // namespace monkeysworld

#endif  // MODEL_H_