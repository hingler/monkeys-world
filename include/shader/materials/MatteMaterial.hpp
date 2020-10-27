#ifndef MATTE_MATERIAL_H_
#define MATTE_MATERIAL_H_

#include <file/FileLoader.hpp>

#include <shader/Material.hpp>
#include <shader/ShaderProgram.hpp>
#include <shader/light/LightDataTemp.hpp>
#include <glm/glm.hpp>

#include <memory>

namespace screenspacemanager {
namespace shader {
namespace materials {

// TODO: Passing in the file loader is OK for now, but if this dependency list grows
//       it may be necessary to build a "context" object and fetch dependencies from that
//       (could also look for a DI package but whatever)

class MatteMaterial : ::screenspacemanager::shader::Material {
 public:
  /**
   *  Creates a new MatteMaterial instance.
   *  @param loader - FileLoader used to fetch files.
   */ 
  MatteMaterial(std::shared_ptr<file::FileLoader> loader);

  /**
   *  Makes the material active and passes all uniforms.
   */ 
  void UseMaterial() override;

  /**
   *  Passes transform data to the respective uniforms.
   *  @param vp_matrix - The view + projection matrices drawn for this
   */ 
  void SetCameraTransforms(const glm::mat4& vp_matrix) override;

  /**
   *  For passing model matrix.
   */ 
  void SetModelTransforms(const glm::mat4& model_matrix) override;

  /**
   *  Passes light data to its respective uniforms. 
   */ 
  void SetLights(const std::vector<light::LightData>& lights) override;

  GLuint GetProgramDescriptor() override;

  /**
   *  Sets the color associated with the material.
   */ 
  void SetSurfaceColor(const glm::vec4& color);

 private:
  ShaderProgram matte_prog_;
};

} // namespace materials
} // namespace shader
} // namespace screenspacemanager

#endif  // MATTE_MATERIAL_H_