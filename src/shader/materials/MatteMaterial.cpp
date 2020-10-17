#include <shader/materials/MatteMaterial.hpp>
#include <shader/ShaderProgram.hpp>
#include <shader/ShaderProgramBuilder.hpp>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <boost/log/trivial.hpp>

namespace screenspacemanager {
namespace shader {
namespace materials {

MatteMaterial::MatteMaterial() {
  matte_prog_ = ShaderProgramBuilder()
                .WithVertexShader("glsl/matte-material/matte-material.vert")
                .WithFragmentShader("glsl/matte-material/matte-material.frag")
                .Build();
}

// GL 4.1 provides glProgramUniform which allows us to bind uniforms
// without having to worry about rebinding the old program
void MatteMaterial::UseMaterial() {
  BOOST_LOG_TRIVIAL(debug) << "New material bound to GL: ";
  BOOST_LOG_TRIVIAL(debug) << matte_prog_.GetProgramDescriptor();
  glUseProgram(matte_prog_.GetProgramDescriptor());
}

void MatteMaterial::SetCameraTransforms(const glm::mat4& vp_matrix) {
  glProgramUniformMatrix4fv(matte_prog_.GetProgramDescriptor(),
                            1,
                            1,
                            GL_FALSE,
                            glm::value_ptr(vp_matrix));
}

void MatteMaterial::SetModelTransforms(const glm::mat4& model_matrix) {
  glProgramUniformMatrix4fv(matte_prog_.GetProgramDescriptor(),
                            0,
                            1,
                            GL_FALSE,
                            glm::value_ptr(model_matrix));
}

void MatteMaterial::SetLights(const std::vector<light::LightData>& lights) {
  if (lights.size() > 0) {
    const light::LightData& light = lights[0];
    GLuint prog = matte_prog_.GetProgramDescriptor();
    glProgramUniform4fv(prog, 4, 1, glm::value_ptr(light.position));
    glProgramUniform1f(prog, 5, light.intensity);
    glProgramUniform4fv(prog, 6, 1, glm::value_ptr(light.diffuse));
    glProgramUniform4fv(prog, 7, 1, glm::value_ptr(light.ambient));
  }
}

void MatteMaterial::SetSurfaceColor(const glm::vec4& color) {
  glProgramUniform4fv(matte_prog_.GetProgramDescriptor(), 4, 1, glm::value_ptr(color));
}

} // namespace materials
} // namespace shader
} // namespace screenspacemanager