#version 430 core

layout(location = 0) in vec2 texcoord;

layout(location = 2) uniform sampler2D glyph_texture;
layout(location = 3) uniform vec4 text_color;

layout(location = 0) out vec4 fragColor;

void main() {
  float texval = texture(glyph_texture, texcoord).r;
  if (texval < 0.05f) {
    discard;
  }
  
  fragColor = texval * text_color;
}