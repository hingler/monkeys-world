#ifndef EMPTY_H_
#define EMPTY_H_

#include <critter/GameObject.hpp>
#include <critter/Context.hpp>

namespace monkeysworld {
namespace critter {

/**
 *  Empties are associated with 
 */ 
class Empty : public GameObject {
 public:
  
  // create a new empty
  Empty(Context* ctx) : GameObject(ctx) {}

  // nop
  void PrepareAttributes() override {}
  void RenderMaterial() override {}
  void Draw() override {}
};


}
}

#endif