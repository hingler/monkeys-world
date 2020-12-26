#include <shader/materials/ShadowMapMaterial.hpp>
#include <shader/ShaderProgramBuilder.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

using critter::Context;

ShadowMapMaterial::ShadowMapMaterial(Context* ctx) {
  shadow_prog_ = ShaderProgramBuilder(ctx->GetCachedFileLoader())
                   .WithVertexShader("resources/glsl/shadow-map/shadow-map.vert")
                   .WithFragmentShader("resources/glsl/shadow-map/shadow-map.frag")
                   .Build();
  
}

void ShadowMapMaterial::UseMaterial() {
  glUseProgram(shadow_prog_.GetProgramDescriptor());
}

void ShadowMapMaterial::SetCameraTransforms(const glm::mat4& vp_matrix) {
  glProgramUniformMatrix4fv(shadow_prog_.GetProgramDescriptor(),
                            1,
                            1,
                            GL_FALSE,
                            glm::value_ptr(vp_matrix));
}

void ShadowMapMaterial::SetModelTransforms(const glm::mat4& model_matrix) {
  glProgramUniformMatrix4fv(shadow_prog_.GetProgramDescriptor(),
                            0,
                            1,
                            GL_FALSE,
                            glm::value_ptr(model_matrix));
}

GLuint ShadowMapMaterial::GetProgramDescriptor() {
  return shadow_prog_.GetProgramDescriptor();
}

}
}
}