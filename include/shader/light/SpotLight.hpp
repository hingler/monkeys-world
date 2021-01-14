#ifndef SPOT_LIGHT_H_
#define SPOT_LIGHT_H_

#include <glad/glad.h>
#include <critter/GameObject.hpp>
#include <engine/Context.hpp>

#include <shader/light/LightTypes.hpp>
#include <shader/light/Light.hpp>
#include <shader/materials/ShadowMapMaterial.hpp>

namespace monkeysworld {
namespace critter {
namespace visitor {

class LightVisitor;

}
}
}

namespace monkeysworld {
namespace shader {
namespace light {

/**
 *  Represents a simple spot light.
 *  Controls similar to a camera.
 *  By default, points along the negative Z axis.
 */ 
class SpotLight : public critter::GameObject, public Light {
  // for accessing framebuffer methods
  friend class ::monkeysworld::critter::visitor::LightVisitor;
 public:
  /**
   *  Constructs a new spotlight
   */ 
  SpotLight(engine::Context* ctx);

  void Accept(critter::Visitor& v) override;

  // nop
  void PrepareAttributes() override {}
  void RenderMaterial(const engine::RenderContext& rc) override {}
  void Draw() override {}

  /**
   *  Returns the matrix associated with this spot light.
   */ 
  glm::mat4 GetLightMatrix();

  /**
   *  Modifies the angle of the spotlight.
   *  Ranges from 0 to 180 degrees, exclusive.
   *  @param deg - the angle, in degrees, of this spot light.
   */ 
  void SetAngle(float deg);

  /**
   *  Generates a spotlight_info struct which represents this spotlight within
   *  a render context.
   */ 
  spotlight_info GetSpotLightInfo();

  materials::ShadowMapMaterial& GetShadowProgram();

 protected:
  /**
   *  Regenerates the framebuffer texture with a new size.
   *  @param px - new framebuffer width + height
   */ 
  void ChangeMapSize(int px);

  /**
   *  Returns descriptor associated with shadow map.
   */ 
  GLuint GetMapDesc();

  /**
   *  Returns descriptor associated with framebuffer.
   */ 
  GLuint GetFramebuffer();

  /**
   *  @returns the width + height of the map, in px.
   */ 
  int GetMapSize();


 private:
  int map_size_;                      // size of shadow map, in px
  GLuint map_;                        // descriptor for map
  GLuint shadow_fb_;                  // framebuffer for shadow map
  materials::ShadowMapMaterial mat_;  // material assc'd with shadow map generation

  float angle_;
};

}
}
}

#endif