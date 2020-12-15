#ifndef SPOT_LIGHT_H_
#define SPOT_LIGHT_H_

#include <glad/glad.h>
#include <critter/GameObject.hpp>
#include <critter/Context.hpp>

namespace monkeysworld {
namespace critter {
namespace visitor {

class LightVisitor;

}
}
}

namespace monkeysworld {
namespace shader {
namespace light {

/**
 *  Represents a simple spot light.
 *  Controls similar to a camera.
 *  By default, points along the negative Z axis.
 */ 
class SpotLight : public critter::GameObject {
  // for accessing framebuffer methods
  friend class ::monkeysworld::critter::visitor::LightVisitor;
 public:
  /**
   *  Constructs a new spotlight
   */ 
  SpotLight(critter::Context* ctx);

  /**
   *  Returns the matrix associated with this spot light.
   */ 
  glm::mat4 GetLightMatrix();

  /**
   *  Modifies the angle of the spotlight.
   *  Ranges from 0 to 180 degrees, exclusive.
   *  @param deg - the angle, in degrees, of this spot light.
   */ 
  void SetAngle(float deg);

 protected:
  /**
   *  Regenerates the framebuffer texture with a new size.
   *  @param px - new framebuffer width + height
   */ 
  void ChangeMapSize(int px);

  /**
   *  Returns descriptor associated with shadow map.
   */ 
  GLuint GetMapDesc();

  /**
   *  Returns descriptor associated with framebuffer.
   */ 
  GLuint GetFramebuffer();

  /**
   *  @returns the width + height of the map, in px.
   */ 
  int GetMapSize();

 private:
  int map_size_;      // size of shadow map, in px
  GLuint map_;        // descriptor for map
  GLuint shadow_fb_;  // framebuffer for shadow map

  float angle_;
};

}
}
}

#endif