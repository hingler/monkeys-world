#version 430 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;

layout(location = 0) in mat4 model_matrix;
layout(location = 1) in mat4 vp_matrix;

layout(location = 0) out vec2 texcoord_output;

void main() {
  texcoord_output = texcoord;
  gl_Position = vp_matrix * model_matrix * position;
}