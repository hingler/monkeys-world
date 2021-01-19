#include <shader/materials/UIGroupMaterial.hpp>

#include <shader/ShaderProgramBuilder.hpp>


namespace monkeysworld {
namespace shader {
namespace materials {

UIGroupMaterial::UIGroupMaterial(engine::Context* ctx) {
  auto loader = ctx->GetCachedFileLoader();
  prog_ = ShaderProgramBuilder(loader)
            .WithVertexShader("resources/glsl/ui-group-mat/ui-group-mat.vert")
            .WithFragmentShader("resources/glsl/ui-group-mat/ui-group-mat.frag")
            .Build();
  
}

void UIGroupMaterial::SetTextures(GLuint textures[TEXTURES_PER_CALL]) {
  for (int i = 0; i < TEXTURES_PER_CALL; i++) {
    textures_[i] = textures[i];
  }
}

void UIGroupMaterial::UseMaterial() {
  glUseProgram(prog_.GetProgramDescriptor());
  
  // prepare textures
  for (int i = 0; i < TEXTURES_PER_CALL; i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, textures_[i]);
    glUniform1i(i, i);
  }
}

GLuint UIGroupMaterial::GetProgramDescriptor() {
  return prog_.GetProgramDescriptor();
}

}
}
}