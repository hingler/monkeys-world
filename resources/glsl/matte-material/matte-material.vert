#version 430 core

// vertex position
layout(location = 0) in vec4 position;

// texture coordinates
layout(location = 1) in vec2 texcoord;

// normals
layout(location = 2) in vec3 normal;

// model transformation matrix
layout(location = 0) uniform mat4 model_matrix;

// premultiplied view-projection matrix
layout(location = 1) uniform mat4 vp_matrix;

// precalculated normal matrix
// inverse transpose of upper left 3x3 of model matrix
layout(location = 2) uniform mat3 normal_matrix;


layout(location = 0) out vec4 position_output;


layout(location = 1) out vec3 normal_output;

void main() {
  position_output = model_matrix * position;
  normal_output = normalize(normal_matrix * normal);
  gl_Position = vp_matrix * model_matrix * position;
}
