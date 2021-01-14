#include <shader/materials/SkyBoxMaterial.hpp>
#include <shader/ShaderProgramBuilder.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

using engine::Context;

SkyBoxMaterial::SkyBoxMaterial(Context* ctx) {
  auto loader = ctx->GetCachedFileLoader();
  skybox_prog_ = ShaderProgramBuilder(loader)
                  .WithVertexShader("resources/glsl/skybox-material/skybox-material.vert")
                  .WithFragmentShader("resources/glsl/skybox-material/skybox-material.frag")
                  .Build();
}

void SkyBoxMaterial::SetViewMatrix(const glm::mat4& view_matrix) {
  view_mat_ = view_matrix;
}

void SkyBoxMaterial::SetPerspectiveMatrix(const glm::mat4& perspective_matrix) {
  pers_mat_ = perspective_matrix;
}

void SkyBoxMaterial::SetCubeMap(CubeMap* map) {
  skybox_cubemap_ = map->GetCubeMapDescriptor();
}

GLuint SkyBoxMaterial::GetProgramDescriptor() {
  return skybox_prog_.GetProgramDescriptor();
}

void SkyBoxMaterial::UseMaterial() {
  glUseProgram(skybox_prog_.GetProgramDescriptor());
  glUniformMatrix4fv(0,
                     1,
                     GL_FALSE,
                     glm::value_ptr(view_mat_));
  
  glUniformMatrix4fv(1,
                     1,
                     GL_FALSE,
                     glm::value_ptr(pers_mat_));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_cubemap_);
  glUniform1i(2, 0);
}



}
}
}