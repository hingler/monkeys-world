#include <shader/materials/FillMaterial.hpp>

#include <shader/ShaderProgramBuilder.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {
using engine::Context;
FillMaterial::FillMaterial() {
  fill_prog_ = ShaderProgramBuilder()
                 .WithVertexShader("resources/glsl/fill-mat/fill-mat.vert")
                 .WithFragmentShader("resources/glsl/fill-mat/fill-mat.frag")
                 .Build();
  color_cache_ = glm::vec4(glm::vec3(0.0), 1.0);
  use_gradient_ = false;
}
FillMaterial::FillMaterial(Context* context) {
  auto exec = context->GetExecutor();
  auto prog_func = [&] {
    fill_prog_ = ShaderProgramBuilder(context->GetCachedFileLoader())
                  .WithVertexShader("resources/glsl/fill-mat/fill-mat.vert")
                  .WithFragmentShader("resources/glsl/fill-mat/fill-mat.frag")
                  .Build();
  };

  exec->ScheduleOnMainThread(prog_func).wait();

  color_cache_ = glm::vec4(glm::vec3(0.0), 1.0);
  use_gradient_ = false;
}

void FillMaterial::UseMaterial() {
  glUseProgram(fill_prog_.GetProgramDescriptor());
  if (use_gradient_) {
    // upload the gradient we have on record
    // use line for now
    auto colors = gradient_.GetStops();
    glUniform1i(32, 0);
    glUniform2f(33, gradient_.start_point.x, gradient_.start_point.y);
    glUniform2f(34, gradient_.end_point.x, gradient_.end_point.y);
    glUniform1i(35, static_cast<int>(colors.size()));

    for (int i = 0; i < colors.size(); i++) {
      glUniform4fv(2 * i, 1, glm::value_ptr(colors[i].col));
      glUniform1f(2 * i + 1, colors[i].stop);
    }
  } else {
    glUniform1i(32, -1);
    glUniform4fv(0, 1, glm::value_ptr(color_cache_));
  }
}

void FillMaterial::SetColor(const glm::vec4& col) {
  color_cache_ = col;
  use_gradient_ = false;
}

void FillMaterial::SetColor(const color::Gradient& grad) {
  gradient_ = grad;
  use_gradient_ = true;
}

}
}
}