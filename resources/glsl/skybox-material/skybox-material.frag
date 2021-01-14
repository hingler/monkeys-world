#version 430 core

layout(location = 0) in vec3 v_Texcoord;

layout(location = 0) out vec4 fragColor;

layout(location = 2) uniform samplerCube u_Cubemap;

void main() {
  fragColor = texture(u_Cubemap, v_Texcoord);
}