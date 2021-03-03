#version 430 core

struct filter_hsl {
  float hue;
  float sat;
  float light;
};

layout(location = 0) in vec2 texcoord;

layout(location = 0) out vec4 fragColor;

layout(location = 0) uniform filter_hsl hsl_filters[6];
layout(location = 18) uniform int filters[16];

layout(location = 34) uniform int filter_count;
layout(location = 35) uniform sampler2D tex_image;

vec4 HSL(vec4 color_in, int filter_index) {
  // do an hsl filter here
  float x_max = max(max(color_in.r, color_in.g), color_in.b);
  float x_min = min(min(color_in.r, color_in.g), color_in.b);
  float x_ran = x_max - x_min;

  float light = (x_max + x_min) / 2.0;
  float hue = 0.0;

  // TODO: reduce all of this branching :(
  if (x_ran == 0) {
    hue = 0.0;
  } else if (x_max == color_in.r) {
    hue = 60.0 * ((color_in.g - color_in.b) / x_ran);
  } else if (x_max == color_in.g) {
    hue = 60.0 * (2.0 + (color_in.b - color_in.r) / x_ran);
  } else { // x_max == color_in.b
    hue = 60.0 * (4.0 + (color_in.r - color_in.g) / x_ran);
  }

  float sat = 0.0;
  if (x_max != 0) {
    sat = (x_ran / x_max);
  }

  // modify by filter specs
  hue += hsl_filters[filter_index].hue;
  sat = max(min(sat + hsl_filters[filter_index].sat, 1.0), 0.0);
  light = max(min(light + hsl_filters[filter_index].light, 1.0), 0.0);

  // convert hue, sat, light back to rgb
  float chroma = (1.0 - abs(2.0 * light - 1.0)) * sat;
  float hue_off = fract(hue / 360.0) * 6.0;
  float x = chroma * (1.0 - abs(mod(hue_off, 2.0) - 1.0));

  vec3 result = vec3(0);
  switch(int(hue_off)) {
    case 0:
      result = vec3(chroma, x, 0);
      break;
    case 1:
      result = vec3(x, chroma, 0);
      break;
    case 2:
      result = vec3(0, chroma, x);
      break;
    case 3:
      result = vec3(0, x, chroma);
      break;
    case 4:
      result = vec3(x, 0, chroma);
      break;
    case 5:
      result = vec3(chroma, 0, x);
      break;
  }

  result += vec3(light - (chroma / 2.0));
  return vec4(result, color_in.a);
}

void main() {

  int hsl_index = 0;

  vec4 color_acc = texture(tex_image, texcoord);
  // for (int i = 0; i < filter_count; i++) {
  //   switch(filters[i]) {
  //     case 0: // hsl
  //       color_acc = HSL(color_acc, hsl_index);
  //       hsl_index = hsl_index + 1;
  //       break;
  //   }
  // }

  fragColor = color_acc;
}