#include <shader/light/Light.hpp>

namespace monkeysworld {
namespace shader {
namespace light {

Light::Light() {
  color_ = glm::vec3(0, 0, 0);
  spec_intensity_ = 0.0;
  diff_intensity_ = 0.0;

  atten_quad_ = 0.0;
  atten_linear_ = 0.0;
  atten_const_ = 0.0;
}

glm::vec3 Light::GetColor() {
  return color_;
}

void Light::SetColor(glm::vec3 col) {
  color_ = col;
}

float Light::GetSpecularIntensity() {
  return spec_intensity_;
}

void Light::SetSpecularIntensity(float intensity) {
  spec_intensity_ = intensity;
}

float Light::GetDiffuseIntensity() {
  return diff_intensity_;
}

void Light::SetDiffuseIntensity(float intensity) {
  diff_intensity_ = intensity;
}

float Light::GetAttenuationQuad() {
  return atten_quad_;
}

float Light::GetAttenuationLinear() {
  return atten_linear_;
}

float Light::GetAttenuationConst() {
  return atten_const_;
}

void Light::SetAttenuationQuad(float atten_quad) {
  atten_quad_ = atten_quad;
}

void Light::SetAttenuationLinear(float atten_linear) {
  atten_linear_ = atten_linear;
}

void Light::SetAttenuationConst(float atten_const) {
  atten_const_ = atten_const;
}

}
}
}