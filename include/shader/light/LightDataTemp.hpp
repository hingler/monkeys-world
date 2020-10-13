#ifndef LIGHT_DATA_TEMP_H_
#define LIGHT_DATA_TEMP_H_

#include <glm/glm.hpp>

namespace screenspacemanager {
namespace shader {
namespace light {

// I'm not sure how to best handle lights right now so instead I'm doing it with a temp struct
// Until I'm more sure how to do so

/**
 *  Simple Point Light struct
 *  No specular component yet
 */ 
struct LightData {
  glm::vec4 position;
  float intensity;
  glm::vec4 diffuse;
  glm::vec4 ambient;
};

} // namespace light
} // namespace shader
} // namespace screenspacemanager

#endif  // LIGHT_DATA_TEMP_H_