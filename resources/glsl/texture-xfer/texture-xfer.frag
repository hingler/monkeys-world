#version 430 core

layout(location = 0) in vec2 texcoord;

layout(location = 0) uniform sampler2D tex;

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = texture(tex, texcoord);
}