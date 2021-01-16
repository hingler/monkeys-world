#version 430 core

layout(location = 0) in vec4 a_Position;

layout(location = 0) out vec3 v_Texcoord;

layout(location = 0) uniform mat4 u_Viewmat;
layout(location = 1) uniform mat4 u_Persmat;

void main() {
  vec4 pos = u_Persmat * u_Viewmat * a_Position;
  v_Texcoord = a_Position.xyz;
  gl_Position = pos.xyww;
}