#ifndef MESH_H_
#define MESH_H_

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <glad/glad.h>

#include <boost/log/trivial.hpp>

#include <model/VertexDataContext.hpp>
#include <model/VertexDataContextGL.hpp>
#include <storage/VertexPacketTypes.hpp>
#include <glm/gtc/constants.hpp>

#include <map>

// from https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature/10707822#10707822
namespace monkeysworld {
namespace model {

/**
 *  Represents all attributes required to reconstruct a mesh.
 *  @tparam Packet - the specification for the vertex data which will be passed in.
 *                   this encompasses data types, and locations.
 */ 
template <typename Packet = storage::VertexPacket3D>
class Mesh {
 private:

  /**
   *  Test struct used to verify that bind method exists
   */ 
  template <typename T> struct HasBind {
    template <typename U, typename void (*)()> struct Tester {};
    template <typename U> static char Test(Tester<U, &U::Bind>*);
    template <typename U> static int Test(...);

    // true if `Bind` method is available, false otherwise
    static constexpr bool Has = (sizeof(Test<T>(NULL)) == sizeof(char));
  };

  static_assert(HasBind<Packet>::Has, "Bind method not available for templated class");
 public:
  /**
   *  Constructs a new VertexData object.
   */ 
  Mesh() : dirty_(true) {
    context_ = std::make_unique<VertexDataContextGL<Packet>>();
  }

  /**
   *  Constructs a new VertexData object from a preallocated context object.
   *  The new object assumes ownership of the passed-in context.
   */ 
  Mesh(std::unique_ptr<VertexDataContext<Packet>> context) :
    dirty_(true),
    context_(std::move(context)) { }

  /**
   *  Stores a new vertex based on the contents of the data packet.
   *  @param packet - data packet containing vertex data.
   */ 
  void AddVertex(const Packet& packet) {
    data_.push_back(packet);
    dirty_ = true;
  }

  /**
   *  Allows direct access to vertices.
   *  @param index - desired index.
   */ 
  Packet& operator[](std::size_t index) {
    dirty_ = true;
    return data_.at(index);
  }

  /**
   *  Stores a new triangle based on the described indices.
   *  @param vertA - First vertex
   *  @param vertB - Second vertex
   *  @param vertC - third vertex
   * 
   *  @return true if the vertices are in-bounds, and false otherwise.
   */ 
  bool AddPolygon(unsigned int vertA, unsigned int vertB, unsigned int vertC) {
    unsigned int min_value = (vertA < vertB ? vertA : vertB);
    unsigned int max_value = (vertA < vertB ? vertB : vertA);
    min_value = (vertC < min_value ? vertC : min_value);
    max_value = (vertC > max_value ? vertC : max_value);
    if (min_value >= 0 && max_value < GetVertexCount()) {
      indices_.push_back(vertA);
      indices_.push_back(vertB);
      indices_.push_back(vertC);
      dirty_ = true;
      return true;
    }

    return false;
  }

  /**
   *  Adds a polygon with an arbitrary number of vertices.
   *  Simple ver: adds n-2 triangles, reading in increasing order.
   *  @param indices - List of vertices in the polygon.
   */
  bool AddPolygon(const std::vector<unsigned int>& indices) {
    // i could try to implement sweep algo
    // http://allenchou.net/2013/12/game-physics-contact-generation-epa/
    for (int i = 0; i < indices.size() - 2; i++) {
      if (!AddPolygon(indices[i], indices[i + 1], indices[i + 2])) {
        return false;
      }
    }

    return true;
  }

  /**
   *  Provides a wrapper for Packet::Bind so that methods working with this data class
   *  need not deduce the template type to bind attributes
   */ 
  void PointToVertexAttribs() const {
    if (dirty_) {
      context_->UpdateBuffersAndPoint(data_, indices_);
    } else {
      context_->Point();
    }

    bool* dirty_noconst = const_cast<bool*>(&dirty_);
    *dirty_noconst = false;
  }

  /**
   *  Returns number of vertices stored here.
   */ 
  size_t GetVertexCount() const {
    return data_.size();
  }

  /**
   *  Returns number of indices currently stored.
   *  (modify: store `polygon` objects instead? and add some functions which can do some quick maths)
   */ 
  size_t GetIndexCount() const {
    return indices_.size();
  }

  /**
   *  Returns a read-only pointer to the underlying vertex data.
   */ 
  const Packet* GetVertexData() const {
    return data_.data();
  }

  /**
   *  Returns a read-only pointer to the underlying index data.
   */ 
  const unsigned int* GetIndexData() const {
    return indices_.data();
  }

  /**
   *  Concatenates the passed Mesh object onto `this`.
   *  @param other - the other Mesh being merged.
   */ 
  void Concatenate(const Mesh<Packet>& other) {
    int index_offset = other.indices_.size();
    for (Packet packet : other.data_) {
      data_.push_back(packet);
    }

    for (int index : other.indices_) {
      indices_.push_back(index + index_offset);
    }

    dirty_ = true;
  }

  Mesh(const Mesh& other) {
    data_ = other.data_;
    indices_ = other.indices_;
    switch (context_->GetType()) {
      case gl:
        context_ = std::make_unique<VertexDataContextGL<Packet>>();
      default:
        context_ = std::unique_ptr<VertexDataContext<Packet>>(nullptr);
    }

    dirty_ = true;
  }

  Mesh& operator=(const Mesh& other) {
    data_ = other.data_;
    indices_ = other.indices_;
    switch (context_->GetType()) {
      case gl:
        context_ = std::make_unique<VertexDataContextGL<Packet>>();
      default:
        context_ = std::unique_ptr<VertexDataContext<Packet>>(nullptr);
    }

    dirty_ = true;

    return *this;
  }

  Mesh(Mesh&& other) {
    // don't remove the context -- reuse mine
    data_ = std::move(other.data_);
    indices_ = std::move(other.indices_);
    dirty_ = true;
  }

  Mesh& operator=(Mesh&& other) {
    data_ = std::move(other.data_);
    indices_ = std::move(other.indices_);
    dirty_ = true;
    return *this;
  }

 private:
  // the underlying data stored.
  std::vector<Packet> data_;

  // list of indices representing polygons formed from our vertices. must be triangles
  std::vector<unsigned int> indices_;

  // context used to make opengl calls
  std::unique_ptr<VertexDataContext<Packet>> context_;

  // tracks whether we need to update our buffers.
  bool dirty_;

  
};

/**
 *  Specialization for 3D packets
 * 
 *  This is mostly working but there are lingering issues -- it's just a backup anyway, so i'll
 *  let it be for now.
 */ 
template <>
bool Mesh<storage::VertexPacket3D>::AddPolygon(const std::vector<unsigned int>& indices) {
  // smart idea: since it's convex, read top to bottom
  // TODO: http://allenchou.net/2013/12/game-physics-contact-generation-epa/
  // someone says this doesn't work (consi)
  if (indices.size() == 3) {
    return AddPolygon(indices[0], indices[1], indices[2]);
  }

  std::vector<std::pair<unsigned int, storage::VertexPacket3D>> vertices;
  for (auto index : indices) {
    if (index >= data_.size() || index < 0) {
      return false;
    }

    vertices.push_back(std::pair<unsigned int,
                       storage::VertexPacket3D>(index, data_[index]));
  }

  glm::vec3 cross_r = vertices[0].second.normals;
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

  int ac = (sorting_axis + 1) % 3;
  int bc = (sorting_axis + 2) % 3;

  // ignore anchor, sort all others in fan order relative to anchor
  std::sort(vertices.begin() + 1,
            vertices.end(),
            [sorting_axis, &anchor, ac, bc]
            (std::pair<unsigned int, storage::VertexPacket3D> a,
             std::pair<unsigned int, storage::VertexPacket3D> b) -> bool {
              // sorting axis = plane of greatest change
              glm::vec3 a_vec = a.second.position - anchor;
              glm::vec3 b_vec = b.second.position - anchor;
              float atan_a = glm::atan(a_vec[bc],
                                       a_vec[ac]);
              float atan_b = glm::atan(b_vec[bc],
                                       b_vec[ac]);

              float diff = atan_b - atan_a;
              if (diff > glm::pi<float>()) {
                // b is very positive, a is negative -- by our rule, b comes before a.
                return false;
              } else if (diff < -glm::pi<float>()) {
                // a is very positive, b is very negative -- by our rule, a comes before b
                return true;
              }
              // both are on the same side of our axes -- return the rational thing.
              return (atan_b > atan_a);
            }
  );

  for (int i = 2; i < vertices.size(); i++) {
    AddPolygon(vertices[0].first, vertices[i - 1].first, vertices[i].first);
  }

  return true;
}
// bool Mesh<storage::VertexPacket2D>::AddPolygon(const std::vector<unsigned int>& indices);

};  // namespace storage
};  // namespace monkeysworld

#endif  // MESH_H_ 
