#include <shader/materials/TextureXferMaterial.hpp>

#include <shader/ShaderProgramBuilder.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

TextureXferMaterial::TextureXferMaterial(engine::Context* context) {
  auto loader = context->GetCachedFileLoader();
  xfer_prog_ = ShaderProgramBuilder(loader)
                 .WithVertexShader("resources/glsl/texture-xfer/texture-xfer.vert")
                 .WithFragmentShader("resources/glsl/texture-xfer/texture-xfer.frag")
                 .Build();
}

void TextureXferMaterial::UseMaterial() {
  glUseProgram(xfer_prog_.GetProgramDescriptor());
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_);
}

void TextureXferMaterial::SetTexture(GLuint tex) {
  tex_ = tex;
}

}
}
}