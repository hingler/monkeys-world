#version 430 core

layout(location = 0) in vec2 position;

void main() {
  gl_Position = vec4(position.xy, -1.0, 1.0);
}