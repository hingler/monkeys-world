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
 public:

  /**
   *  Creates a new Model with no vertex data attached.
   *  @param ctx - pointer to the relevant context.
   */ 
  Model(Context* ctx);

  /**
   *  Sets the mesh associated with this model instance.
   *  @param mesh - shared ptr to a 3D mesh object.
   */ 
  void SetMesh(const std::shared_ptr<model::Mesh<>>& mesh);
  
  /**
   *  Returns ptr to the current mesh.
   */ 
  std::shared_ptr<model::Mesh<>> GetMesh();

  Model(const Model& other);
  Model(Model&& other);
  Model& operator=(const Model& other);
  Model& operator=(Model&& other);

  /**
   *  Loads vertex data from a .OBJ file and adds it to shader.
   * 
   *  @param ctx - pointer to the relevant context.
   *  @param path - path to the desired OBJ file.
   *  @returns newly created mesh.
   */ 
  static std::shared_ptr<model::Mesh<>> FromObjFile(Context* ctx, const std::string& path);

 private:
  std::shared_ptr<model::Mesh<>> mesh_;
};

} // namespace critter
} // namespace monkeysworld

#endif  // MODEL_H_