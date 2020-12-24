#include <shader/light/SpotLight.hpp>

#include <critter/Visitor.hpp>

#include <shader/materials/ShadowMapMaterial.hpp>
#include <shader/Material.hpp>

#include <memory>

namespace monkeysworld {
namespace critter {
namespace visitor {

/**
 *  Visits components in the component hierarchy and
 *  renders their contents to the desired spot light shadow map.
 */ 
class SpotLightMapVisitor : public critter::Visitor {
 public:
  /**
   *  Creates a new SpotLightMapVisitor, associated with the passed-in light.
   *  @param light - the light associated with this visitor
   */ 
  SpotLightMapVisitor(std::shared_ptr<shader::light::SpotLight> light);

  void Visit(critter::Object* o) override {}

  void Visit(critter::GameObject* o) override {
    // bind, activate material, then draw
    o->PrepareAttributes();
    light->GetShadowProgram().SetCameraTransforms(light->GetLightMatrix());
    light->GetShadowProgram().SetModelTransforms(o->GetTransformationMatrix());
    o->Draw();
  }

  void Visit(critter::GameCamera* o) override {}
  void Visit(shader::light::SpotLight* o) override {}
 private:
  std::shared_ptr<shader::light::SpotLight> light;

};

}
}
}