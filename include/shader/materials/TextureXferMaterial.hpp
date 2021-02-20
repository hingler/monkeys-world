#ifndef TEXTURE_XFER_MATERIAL_H_
#define TEXTURE_XFER_MATERIAL_H_

#include <shader/Material.hpp>
#include <shader/ShaderProgram.hpp>

#include <engine/Context.hpp>

namespace monkeysworld {
namespace shader {
namespace materials {

/**
 *  Draws a texture directly to the screen, based on texcoords.
 */ 
class TextureXferMaterial : public shader::Material {
 public:
  TextureXferMaterial(engine::Context* context);

  void UseMaterial() override;

  /**
   *  Sets the texture which will be drawn onto the screen.
   *  @param tex - texture descriptor for onscreen texture.
   */ 
  void SetTexture(GLuint tex);
  void SetOpacity(float opac);
 private:
  GLuint tex_;
  float opac_;
  ShaderProgram xfer_prog_;
};

}
}
}

#endif