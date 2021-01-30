#version 430 core

layout(location = 0) in vec2 v_texcoord;

// resolution of the framebuffer
layout(location = 0) uniform vec2 u_resolution;

layout(location = 1) uniform float border_width;

layout(location = 2) uniform float border_radius;

layout(location = 3) uniform vec4 button_color;

layout(location = 4) uniform vec4 border_color;

layout(location = 0) out vec4 fragcolor;

float button_fill(float dist_from_edge) {
  // center is u_resolution / 2
  // abs function is center - button_dims / 2
  vec2 center = u_resolution / 2;
  vec2 button_dims = center - dist_from_edge;
  vec2 coord = gl_FragCoord;
  float x_abs = abs(coord.x - center.x) - button_dims.x;
  float y_abs = abs(coord.y - center.y) - button_dims.y;
  return min(step(x_abs, 0)
           + step(y_abs, 0)
           + step(x_abs * x_abs + y_abs * y_abs, border_radius * border_radius), 1);
}

void main() {
  vec4 output = vec4(0);
  float fill_outer = button_fill(0.0);
  float fill_inner = button_fill(border_width);
  fill_outer = min(1 - fill_inner, fill_outer);
  fragcolor = button_color * fill_inner + border_color * fill_outer;
}