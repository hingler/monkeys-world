#version 430 core

layout(location = 0) in vec3 texture;

layout(location = 0) out vec4 fragColor;

layout(location = 2) uniform samplerCube skybox;

void main() {
  fragColor = texture(skybox, texture);
}