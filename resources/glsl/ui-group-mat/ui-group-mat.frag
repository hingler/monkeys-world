#version 430 core

#define TEXTURES_PER_CALL 4

precision mediump float;

layout(location = 0) in sampler2D textures[4];

layout(location = 0) in vec2 v_tex;
layout(location = 1) in float v_ind;

layout(location = 0) out vec4 fragColor;

void main() {
  vec4 result = vec4(0);
  vec4 tex_content;
  for (int i = 0; i < TEXTURES_PER_CALL; i++) {
    tex_content = texture(textures[i], v_tex);
    if (int(v_ind) == i) {
      result += tex_content;
    }
  }

  fragColor = result;
}