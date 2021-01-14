#include <shader/materials/SkyboxMaterial.hpp>

#include <shader/ShaderProgramBuilder.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

SkyboxMaterial::SkyboxMaterial(engine::Context* context) {
  auto loader = context->GetCachedFileLoader();
  skybox_prog_ = ShaderProgramBuilder(loader)
                  .WithVertexShader("resources/glsl/skybox-material/skybox-material.vert")
                  .WithFragmentShader("resources/glsl/skybox-shader/skybox-shader.frag")
                  .Build();
}

void SkyboxMaterial::UseMaterial() {
  auto prog = skybox_prog_.GetProgramDescriptor();
  glUseProgram(prog);
  // TODO: we could do some shit in the setters like checking if the current context is bound
  glProgramUniform4fv(prog, 0, 1, glm::value_ptr(view_mat_));
  glProgramUniform4fv(prog, 1, 1, glm::value_ptr(model_mat_));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_);
  
  glProgramUniform1i(prog, 2, cube_map_);
}

void SkyboxMaterial::SetCameraView(const glm::mat4& view_mat) {
  // strips translation component
  view_mat_ = glm::mat4(glm::mat3(view_mat));
}

void SkyboxMaterial::SetCameraPersp(const glm::mat4& model_mat) {
  model_mat_ = model_mat;
}

void SkyboxMaterial::SetCubeMap(GLuint cube_map) {
  cube_map_ = cube_map;
}

GLuint SkyboxMaterial::GetProgramDescriptor() {
  return skybox_prog_.GetProgramDescriptor();
}

}
}
}