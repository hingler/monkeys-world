#version 430 core

precision mediump float;

layout(location = 0) in vec2 a_pos;
layout(location = 1) in vec2 a_tex;
layout(location = 2) in float a_ind;

layout(location = 0) out vec2 v_tex;
layout(location = 1) out float v_ind;

void main() {
  v_ind = a_ind;
  v_tex = a_tex;
  gl_Position = vec4(a_pos, 0.0, 1.0);
}