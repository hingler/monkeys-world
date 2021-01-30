#include <shader/materials/ButtonMaterial.hpp>
#include <shader/ShaderProgramBuilder.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

using engine::Context;

ButtonMaterial::ButtonMaterial(Context* ctx) {
  auto loader = ctx->GetCachedFileLoader();
  prog_ = ShaderProgramBuilder(loader)
            .WithVertexShader("resources/glsl/button-material/button-material.vert")
            .WithFragmentShader("resources/glsl/button-material/button-material.frag")
            .Build();

  border_width = 1.0;
  border_radius = 0.0;
  button_color = glm::vec4(glm::vec3(0.8), 1.0);
  border_color = glm::vec4(glm::vec3(0.4), 1.0);
}

void ButtonMaterial::UseMaterial() {
  glUseProgram(prog_.GetProgramDescriptor());
  glUniform2fv(0, 1, glm::value_ptr(resolution));
  glUniform1f(1, border_width);
  glUniform1f(2, border_radius);
  glUniform4fv(3, 1, glm::value_ptr(button_color));
  glUniform4fv(4, 1, glm::value_ptr(border_color));
}

}
}
}