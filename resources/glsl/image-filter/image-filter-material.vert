#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 v_texcoord;

void main() {
  v_texcoord = texcoord;
  gl_Position = vec4(position.xy, -1, 1);
}