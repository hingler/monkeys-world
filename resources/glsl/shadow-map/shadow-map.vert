#version 430 core

layout (location = 0) in vec4 position;

layout (location = 0) uniform mat4 model_mat;
layout (location = 1) uniform mat4 light_mat;

void main() {
  gl_Position = light_mat * model_mat * position;
}