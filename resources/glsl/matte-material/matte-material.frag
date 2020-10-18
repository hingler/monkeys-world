#version 430 core

struct Light {
  vec4 pos;               // position of the light
  float intensity;        // intensity of the light
  vec4 diffuse;           // diffuse color
  vec4 ambient;           // ambient color + intensity
};

layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

layout(location = 3) uniform vec4 surface_color;
layout(location = 4) uniform Light light;  // 4 (pos) - 7 (ambient)

layout(location = 0) out vec4 fragColor;

void main() {
  vec4 light_vector = light.pos - position;
  float dist = length(light_vector);
  light_vector = normalize(light_vector);
  float n_b = max(dot(light_vector.xyz, normal), 0.0);
  // no attenuation yet
  vec4 col = surface_color * (n_b * light.intensity);
  fragColor = col;
}