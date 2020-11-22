#ifndef MODEL_H_
#define MODEL_H_

#include <critter/GameObject.hpp>
#include <critter/Context.hpp>
#include <model/Mesh.hpp>
#include <storage/VertexPacketTypes.hpp>

#include <memory>

namespace monkeysworld {
namespace critter {

/**
 *  A type of GameObject which contains visible geometry.
 */ 
class Model : public GameObject {
  // TODO: template seems a bit clunky for handling custom packets.
  //       it would work ofc, but i think if there's an alternative soln i would rather do that.
  //       if i need it later, do it.
 public:

  /**
   *  Creates a new Model with no vertex data attached.
   *  @param ctx - pointer to the relevant context.
   */ 
  Model(Context* ctx);

  /**
   *  Binds VAO associated with vertices.
   */ 
  void PrepareAttributes() override;

  /**
   *  Draws the object onto the screen.
   */ 
  void RenderMaterial() override;

  /**
   *  Sets the mesh associated with this model instance.
   *  @param mesh - shared ptr to a 3D mesh object.
   */ 
  void SetMesh(const std::shared_ptr<model::Mesh<>>& mesh);

  Model(const Model& other);
  Model(Model&& other);
  Model& operator=(const Model& other);
  Model& operator=(Model&& other);

  /**
   *  Loads vertex data from a .OBJ file and adds it to shader.
   * 
   *  @param ctx - pointer to the relevant context.
   *  @param path - path to the desired OBJ file.
   */ 
  static std::shared_ptr<Model> FromObjFile(Context* ctx, const std::string& path);

 private:
  // TODO: need to handle textures as well!
  std::shared_ptr<model::Mesh<>> mesh_;
};

} // namespace critter
} // namespace monkeysworld

#endif  // MODEL_H_