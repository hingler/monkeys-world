#ifndef SPOT_LIGHT_H_
#define SPOT_LIGHT_H_

#include <critter/GameObject.hpp>

namespace monkeysworld {
namespace shader {
namespace light {

/**
 *  Represents a simple spot light.
 *  Controls similar to a camera.
 *  By default, points along the negative Z axis.
 */ 
class SpotLight : public critter::GameObject {
 public:
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

  // 
};

}
}
}

#endif