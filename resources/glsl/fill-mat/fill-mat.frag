#version 430 core

#define INVALID -1
#define LINE 0

struct GradientStop {
  vec4 color;
  float stop;
};

struct Gradient {
  int gradient_type;
  vec2 start;
  vec2 end;
  int color_count;
};

layout(location = 0) uniform GradientStop colors[16];
layout(location = 32) uniform Gradient gradient;

layout(location = 0) out vec4 fragColor;

float fade(float t) {
  return t * t * t * (t * (t * 6 - 15) + 10);
}

vec4 calculateLineGradient(Gradient grad) {
  // use frag color to get position along gradient
  vec2 dist_max = grad.end - grad.start;
  vec2 dist_actual = gl_FragCoord.xy - grad.start;
  float stop = dot(normalize(dist_max), normalize(dist_actual)) * (length(dist_actual) / length(dist_max));
  // we are before the first stop
  if (colors[0].stop > stop) {
    return colors[0].color;
  }

  for (int i = 1; i < grad.color_count; i++) {
    if (colors[i].stop > stop) {
      float stop_mix = (stop - colors[i - 1].stop) / (colors[i].stop - colors[i - 1].stop);
      return mix(colors[i - 1].color, colors[i].color, stop_mix);
    }
  }

  // no stop was greater than our calculated stop
  // we are past the end
  return colors[grad.color_count - 1].color;
}

void main() {
  switch (gradient.gradient_type) {
    case INVALID:
      fragColor = colors[0].color;
      break;
    case LINE:
      fragColor = calculateLineGradient(gradient);
      break;
  }
}