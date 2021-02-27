#include <shader/materials/SkyboxMaterial.hpp>

#include <shader/ShaderProgramBuilder.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

SkyboxMaterial::SkyboxMaterial(engine::Context* context) {
  auto loader = context->GetCachedFileLoader();

  auto exec_func = [&] {
    skybox_prog_ = ShaderProgramBuilder(loader)
                    .WithVertexShader("resources/glsl/skybox-material/skybox-material.vert")
                    .WithFragmentShader("resources/glsl/skybox-material/skybox-material.frag")
                    .Build();
  };

  auto f = context->GetExecutor()->ScheduleOnMainThread(exec_func);
  f.wait();
}

void SkyboxMaterial::UseMaterial() {
  auto prog = skybox_prog_.GetProgramDescriptor();
  glUseProgram(prog);
  glProgramUniformMatrix4fv(prog, 0, 1, GL_FALSE, glm::value_ptr(view_mat_));
  glProgramUniformMatrix4fv(prog, 1, 1, GL_FALSE, glm::value_ptr(model_mat_));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cube_map_);
  glProgramUniform1i(prog, 2, 0);
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

}
}
}