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
  for (int i = 0; i < TEXTURES_PER_CALL; i++) {
    opacities_[i] = 1.0f;
  }
  
}

void UIGroupMaterial::SetTextures(GLuint textures[TEXTURES_PER_CALL]) {
  for (int i = 0; i < TEXTURES_PER_CALL; i++) {
    textures_[i] = textures[i];
  }
}

void UIGroupMaterial::SetOpacity(float opacities[TEXTURES_PER_CALL]) {
  for (int i = 0; i < TEXTURES_PER_CALL; i++) {
    if (opacities[i] < 0.0f) {
      opacities_[i] = 0.0f;
    } else if (opacities[i] > 1.0f) {
      opacities_[i] = 1.0f;
    } else {
      opacities_[i] = opacities[i];
    }
  }
}

void UIGroupMaterial::UseMaterial() {
  glUseProgram(prog_.GetProgramDescriptor());
  
  // prepare textures
  for (int i = 0; i < TEXTURES_PER_CALL; i++) {
    // todo: deal with this
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, textures_[i]);
    glUniform1i(i, i);
  }

  glUniform1fv(4, 4, opacities_);
}

}
}
}