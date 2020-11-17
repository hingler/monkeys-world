#include <critter/Model.hpp>
#include <critter/GameObject.hpp>

namespace monkeysworld {
namespace critter {

Model::Model(Context* ctx) : GameObject(ctx) {}

void Model::PrepareAttributes() {
  mesh_->PointToVertexAttribs();
}

void Model::SetMesh(const std::shared_ptr<model::Mesh<>>& mesh) {
  mesh_ = mesh;
}

Model Model::FromObjFile(Context* ctx, const std::string& path) {
  // ignore pound sign lines
  // v: position (w is opt, def to 1.0, but check)
  // vt: tex coord (v, w are opt -- def to 0)
  // vn: normal (x, y, z -- might not be normal!)
  // vp: free form (can ignore)
  // f: faces
  //    - v/t/n
  //    - v only
  //    - v/t
  //    - v//n
  // probably ignore material blocks

  // rationale:

  //    - list of vec3s for v, vt, vn
  //    - list of triplets for faces
  //    - create struct for v/t/n triplets
  //    - need some data type which is both like a set and a vector
  //      - quick lookup...
  //      - but while maintaining insertion order.
  //      - unordered_map for triplets -> indices
  //      - vector for stored indices
  //      - when reading a triplet of face values...
  //        - check the map to see if the triplet exists
  //        - if it does: output the index to the vector
  //        - if it does not: push to the map, insert a new element.
  //        - increment the "num_indices" count, or use map size (former seems quicker)
  //        
}

}
}