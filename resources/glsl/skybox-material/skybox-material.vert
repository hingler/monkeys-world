#version 430 core

layout(location = 0) in vec4 position;

layout(location = 0) out vec3 texture;

// view mat with transform stripped out
layout(location = 0) uniform mat4 v_mat;

// perspective mat
layout(location = 1) uniform mat4 p_mat;

void main() {
  vec4 pos = p_mat * v_mat * position;
  texture = pos.xyz;
  gl_Position = pos.xyww;
}