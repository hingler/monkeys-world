#ifndef LIGHT_H_
#define LIGHT_H_

#include <glm/glm.hpp>

namespace monkeysworld {
namespace shader {
namespace light {

/**
 *  Defines basic properties of a GL light.
 */ 
class Light {

 public:
  Light();

  /**
   *  Controls the color of this light.
   */ 
  glm::vec3 GetColor();
  void SetColor(glm::vec3 col);

  /**
   *  Intensity of the specular highlights.
   */ 
  float GetSpecularIntensity();
  void SetSpecularIntensity(float intensity);

  /**
   *  Intensity of diffuse fill.
   */ 
  float GetDiffuseIntensity();
  void SetDiffuseIntensity(float intensity);

  /**
   *  Attenuation -- how brightness changes with distance.
   */ 
  float GetAttenuationQuad();
  float GetAttenuationLinear();
  float GetAttenuationConst();

  void SetAttenuationQuad(float atten_quad);
  void SetAttenuationLinear(float atten_linear);
  void SetAttenuationConst(float atten_const);
 private:
  glm::vec3 color_;
  float spec_intensity_;
  float diff_intensity_;
  
  float atten_quad_;
  float atten_linear_;
  float atten_const_;
};

}
}
}

#endif