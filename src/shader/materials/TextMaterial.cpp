#include <shader/materials/TextMaterial.hpp>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

using file::FileLoader;
using engine::Context;

TextMaterial::TextMaterial(Context* context) {
  std::shared_ptr<FileLoader> loader = std::static_pointer_cast<FileLoader>(context->GetCachedFileLoader());
  text_prog_ = ShaderProgramBuilder(loader)
                 .WithVertexShader("resources/glsl/text-material/text-material.vert")
                 .WithFragmentShader("resources/glsl/text-material/text-material.frag")
                 .Build();
}

void TextMaterial::UseMaterial() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glProgramUniform1i(text_prog_.GetProgramDescriptor(), 2, 0);
  glUseProgram(text_prog_.GetProgramDescriptor());
}

void TextMaterial::SetCameraTransforms(const glm::mat4& vp_matrix) {
  glProgramUniformMatrix4fv(text_prog_.GetProgramDescriptor(),
                            1,
                            1,
                            GL_FALSE,
                            glm::value_ptr(vp_matrix));
}

void TextMaterial::SetModelTransforms(const glm::mat4& model_matrix) {
  glProgramUniformMatrix4fv(text_prog_.GetProgramDescriptor(),
                            0,
                            1,
                            GL_FALSE,
                            glm::value_ptr(model_matrix));
}

GLuint TextMaterial::GetProgramDescriptor() {
  return text_prog_.GetProgramDescriptor();
}

void TextMaterial::SetTextColor(const glm::vec4& color) {
  glProgramUniform4fv(text_prog_.GetProgramDescriptor(),
                      3,
                      1,
                      glm::value_ptr(color));
}

void TextMaterial::SetGlyphTexture(GLuint tex) {
  texture_ = tex;
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture_);
  glProgramUniform1i(text_prog_.GetProgramDescriptor(), 2, 0);
}

}
}
}