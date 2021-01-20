#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texcoord;

layout(location = 0) out vec2 tex;

void main() {
  tex = texcoord;
  gl_Position = position;
}