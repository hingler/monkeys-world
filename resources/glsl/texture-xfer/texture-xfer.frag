#version 430 core

layout(location = 0) in vec2 texcoord;

layout(location = 0) uniform sampler2D tex;
layout(location = 1) uniform float opacity;

layout(location = 0) out vec4 fragColor;

void main() {
  vec4 lookup = texture(tex, texcoord);
  lookup.a = lookup.a * opacity;
  fragColor = lookup;
}