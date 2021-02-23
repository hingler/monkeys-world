#include <shader/materials/TextureXferMaterial.hpp>

#include <shader/ShaderProgramBuilder.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

TextureXferMaterial::TextureXferMaterial(engine::Context* context) {
  auto loader = context->GetCachedFileLoader();
  auto exec_func = [&] {
    xfer_prog_ = ShaderProgramBuilder(loader)
                  .WithVertexShader("resources/glsl/texture-xfer/texture-xfer.vert")
                  .WithFragmentShader("resources/glsl/texture-xfer/texture-xfer.frag")
                  .Build();
  };

  auto f = context->GetExecutor()->ScheduleOnMainThread(exec_func);
  f.wait();

  opac_ = 1.0f;
}

void TextureXferMaterial::UseMaterial() {
  glUseProgram(xfer_prog_.GetProgramDescriptor());
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_);
  glUniform1i(0, 0);
  glUniform1f(1, opac_);
}

void TextureXferMaterial::SetTexture(GLuint tex) {
  tex_ = tex;
}

void TextureXferMaterial::SetOpacity(float opac) {
  if (opac < 0.0f) {
    opac_ = 0.0f;
  } else if (opac > 1.0f) {
    opac_ = 1.0f;
  } else {
    opac_ = opac;
  }
}

}
}
}