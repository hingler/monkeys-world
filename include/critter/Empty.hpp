#ifndef EMPTY_H_
#define EMPTY_H_

#include <critter/GameObject.hpp>
#include <engine/Context.hpp>

namespace monkeysworld {
namespace critter {

/**
 *  Empties are associated with 
 */ 
class Empty : public GameObject {
 public:
  
  // create a new empty
  Empty(engine::Context* ctx) : GameObject(ctx) {}

  // nop
  void PrepareAttributes() override {}
  void RenderMaterial(const engine::RenderContext& rc) override {}
  void Draw() override {}
};


}
}

#endif