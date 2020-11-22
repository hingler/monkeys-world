#include <model/Mesh.hpp>

#include <map>

#include <glm/gtc/constants.hpp>

namespace monkeysworld {
namespace model {

using storage::VertexPacket3D;

typedef std::pair<unsigned int, VertexPacket3D> VertexMap3D;

bool Mesh<VertexPacket3D>::AddPolygon(const std::vector<unsigned int>& indices) {
  // smart idea: since it's convex, read top to bottom
  if (indices.size() == 3) {
    return AddPolygon(indices[0], indices[1], indices[2]);
  }

  std::vector<VertexMap3D> vertices;
  for (auto index : indices) {
    if (index >= data_.size()) {
      return false;
    }

    vertices.push_back(VertexMap3D(index, data_[index]));
  }

  glm::vec3 cross_a = vertices[2].second.position - vertices[1].second.position;
  glm::vec3 cross_b = vertices[0].second.position - vertices[1].second.position;
  glm::vec3 cross_r = glm::cross(cross_a, cross_b);
  // read along shortest axis
  int sorting_axis;
  if (cross_r.x > cross_r.y) {
    if (cross_r.x > cross_r.z) {
      sorting_axis = 0;   // x axis
    } else {
      sorting_axis = 2;
    }
  } else if (cross_r.y > cross_r.z) {
    sorting_axis = 1;
  } else {
    sorting_axis = 2;
  }

  glm::vec3 anchor = vertices[0].second.position;

  int ac = (sorting_axis - 1) % 3;
  int bc = (sorting_axis + 1) % 3;

  // ignore anchor, sort all others in fan order relative to anchor
  std::sort(vertices.begin() + 1,
            vertices.end(),
            [sorting_axis, &anchor, ac, bc](VertexMap3D a, VertexMap3D b) -> bool {
              // sorting axis = plane of greatest change
              glm::vec3 a_vec = a.second.position - anchor;
              glm::vec3 b_vec = b.second.position - anchor;
              float atan_a = glm::atan(a.second.position[ac],
                                       a.second.position[bc]);
              float atan_b = glm::atan(b.second.position[ac],
                                       b.second.position[bc]);
              if (glm::abs(atan_b - atan_a) > glm::pi<float>()) {
                atan_b = glm::mod(atan_b + glm::two_pi<float>(), glm::two_pi<float>());
                atan_a = glm::mod(atan_a + glm::two_pi<float>(), glm::two_pi<float>());
              }
              
              // CCW order!
              return (atan_b > atan_a);
            }
  );

  for (int i = 2; i < vertices.size(); i++) {
    AddPolygon(vertices[0].first, vertices[i - 1].first, vertices[i].first);
  }

}

}
}