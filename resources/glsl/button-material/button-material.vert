#version 430 core

layout(location = 0) in vec4 a_pos;

layout(location = 0) out vec2 v_texcoord;

void main() {
  v_texcoord = (a_pos.xy + vec2(1.0)) / 0.5;
  gl_Position = a_pos;
}