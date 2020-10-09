#ifndef MATTE_MATERIAL_H_
#define MATTE_MATERIAL_H_

#include <shader/Material.hpp>
#include <shader/ShaderProgram.hpp>

namespace screenspacemanager {
namespace shader {
namespace materials {

class MatteMaterial : ::screenspacemanager::shader::Material {
 public:
  /**
   *  Creates a new MatteMaterial instance.
   */ 
  MatteMaterial();

  /**
   *  Makes the material active and passes all uniforms.
   */ 
  void UseMaterial() override;

  /**
   *  Passes transform data to the respective uniforms
   */ 
  void SetCameraTransforms(const glm::mat4& vp_matrix) override;

  /**
   * Set various uniform values.
   */ 
  void SetColor(const glm::vec4& color);            // surface color
  void SetAmbient(const glm::vec4& ambient_color);  // ambient color
  void SetDiffuse(const glm::vec4& diffuse_color);  // diffuse color
  // super temporary!!!
  void SetLightPosition(const glm::vec3& pos);      // light position

 private:
  ShaderProgram matte_prog_;
};

}
}
}

#endif  // MATTE_MATERIAL_H_