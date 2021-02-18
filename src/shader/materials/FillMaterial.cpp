#include <shader/materials/FillMaterial.hpp>

#include <shader/ShaderProgramBuilder.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {
using engine::Context;
FillMaterial::FillMaterial() {
  fill_prog_ = ShaderProgramBuilder()
                 .WithVertexShader("resources/glsl/fill-mat.vert")
                 .WithFragmentShader("resources/glsl/fill-mat.frag")
                 .Build();
  color_cache_ = glm::vec4(glm::vec3(0.0), 1.0);
}
FillMaterial::FillMaterial(Context* context) {
  auto loader = context->GetCachedFileLoader();
  fill_prog_ = ShaderProgramBuilder(loader)
                 .WithVertexShader("resources/glsl/fill-mat.vert")
                 .WithFragmentShader("resources/glsl/fill-mat.frag")
                 .Build();
  color_cache_ = glm::vec4(glm::vec3(0.0), 1.0);
}

void FillMaterial::UseMaterial() {
  glUseProgram(fill_prog_.GetProgramDescriptor());
  glUniform4fv(0, 4, glm::value_ptr(color_cache_));
}

void FillMaterial::SetColor(const glm::vec4& col) {
  color_cache_ = col;
}

}
}
}