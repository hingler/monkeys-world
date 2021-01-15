#ifndef SKYBOX_H_
#define SKYBOX_H_

#include <critter/GameObject.hpp>

#include <shader/CubeMap.hpp>

#include <shader/materials/SkyboxMaterial.hpp>

#include <model/Mesh.hpp>

namespace monkeysworld {
namespace critter {

/**
 *  Skyboxes, as the name suggests, represent skyboxes.
 *  A skybox is created from a cubemap. Skyboxes project infinitely in all directions, by design.
 */ 
class Skybox : public GameObject {
 public:

  /**
   *  Creates a new Skybox.
   */ 
  Skybox(engine::Context* ctx);

  /**
   *  Creates a new Skybox and sets its cubemap to a provided one.
   *  Equivalent to constructing a new skybox and calling SetCubeMap on it.
   *  @param c - the new cubemap.
   */ 
  Skybox(engine::Context* ctx, std::shared_ptr<shader::CubeMap> c);

  /**
   *  Override for visitor functions.
   */ 
  // void Accept(Visitor& v) override;

  /**
   *  Sets the cubemap associated with this Skybox.
   *  @param c - the new cubemap.
   */ 
  void SetCubeMap(std::shared_ptr<shader::CubeMap> c);

  // TODO: if we add a tag indicating which render pass we're running,
  //       then we don't need to separate out all of these steps, except for convenience.

  //       we could have a single "Render" method which handles things appropriately

  //       i might just leave them here because they're nice to have :)
  void PrepareAttributes() override;
  void RenderMaterial(const engine::RenderContext& rc) override;
  void Draw() override;
 private:
  // the mesh is only needed internally, so i'm fine with writing some custom shit for it
  
  struct PositionPacket {
    glm::vec3 position;

    static void Bind();
  };

  model::Mesh<PositionPacket> mesh_;

  shader::materials::SkyboxMaterial mat_;
  std::shared_ptr<shader::CubeMap> cube_map_;
};

}
}

#endif