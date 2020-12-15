#ifndef LIGHT_TYPES_H_
#define LIGHT_TYPES_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace monkeysworld {
namespace shader {
namespace light {

/**
 *  Enums representing each type of light.
 *  Used by the engine and the render context to receive
 *  specific light types.
 */ 
enum LightTypes {
  SPOTLIGHT
};

struct frame_info {
  int width;          // fb width
  int height;         // fb height
  GLuint map;         // shadow map fd
};

/**
 *  Render context's representation of a spotlight.
 */ 
struct spotlight_info {
  glm::mat4 spotlight_view_matrix;      // view matrix for drawing spotlight
  
  glm::vec3 color;                      // spotlight color

  float spec_falloff;                   // spotlight specularity (32.0 = smooth, lower = rougher)

  float intensity_spec;                 // intensity of specular
  float intensity_diff;                 // intensity of diffuse

  // direction can be inferred from spotlight texture can't it?
  // if the shadow map is square, and we know the fov, then the pixel dist from the center
  // tells us the angle from center.

  // well... for someone else's sake, let's include it.

  glm::vec3 direction;                  // direction spotlight is facing
  glm::vec3 position;                   // position of spotlight

  // attenuation -- for light falloff
  float atten_quad;
  float atten_linear;
  float atten_const;

  frame_info fd;
};

}
}
}

#endif