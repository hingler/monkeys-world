#ifndef TEXT_MATERIAL_H_
#define TEXT_MATERIAL_H_

#include <file/CachedFileLoader.hpp>

#include <shader/Material.hpp>
#include <shader/ShaderProgram.hpp>
#include <shader/ShaderProgramBuilder.hpp>

#include <engine/Context.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

class TextMaterial : public ::monkeysworld::shader::Material {
 public:
  /**
   *  New material ctor.
   */ 
  TextMaterial(engine::Context* context);

  void UseMaterial() override;

  void SetCameraTransforms(const glm::mat4& vp_matrix);
  void SetModelTransforms(const glm::mat4& model_matrix);

  void SetLights(const std::vector<light::LightData>& lights) {}
  void SetSpotlights(const std::vector<light::spotlight_info>& lights) {}
  GLuint GetProgramDescriptor() override;

  void SetTextColor(const glm::vec4& color);
  void SetGlyphTexture(GLuint tex);

 private:
  ShaderProgram text_prog_;
  GLuint texture_;
};

}
}
}

#endif