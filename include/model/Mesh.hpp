#ifndef MESH_H_
#define MESH_H_

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <glad/glad.h>

#include <boost/log/trivial.hpp>

#include <model/VertexDataContext.hpp>

// from https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature/10707822#10707822
// todo: enable some R/W on attributes (not sure how yet lol)

namespace screenspacemanager {
namespace model {

/**
 *  Represents all attributes required to reconstruct a mesh.
 *  @tparam Packet - the specification for the vertex data which will be passed in.
 *                   this encompasses data types, and locations.
 */ 
template <typename Packet>
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
    context_ = std::make_unique<::screenspacemanager::model::VertexDataContextGL<Packet>>();
   }

  /**
   *  Constructs a new VertexData object from a preallocated context object.
   *  The new object assumes ownership of the passed-in context.
   */ 
  Mesh(std::unique_ptr<::screenspacemanager::model::VertexDataContext<Packet>> context) :
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
   *  Stores a new triangle based on the described indices.
   *  @param vertA - First vertex
   *  @param vertB - Second vertex
   *  @param vertC - third vertex
   * 
   *  @return true if the vertices are in-bounds, and false otherwise.
   */ 
  bool AddPolygon(unsigned int vertA, unsigned int vertB, unsigned int vertC) {
    std::initializer_list<unsigned int> vert_list({vertA, vertB, vertC});
    int min_value = std::min(vert_list);
    int max_value = std::max(vert_list);
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
   *  Provides a wrapper for Packet::Bind so that methods working with this data class
   *  need not deduce the template type to bind attributes
   */ 
  void PointToVertexAttribs() {
    if (dirty_) {
      BOOST_LOG_TRIVIAL(debug) << "Updating GPU buffers";
      context_->UpdateBuffersAndPoint(data_, indices_);
    } else {
      BOOST_LOG_TRIVIAL(debug) << "Pointing only";
      context_->Point();
    }

    dirty_ = false;
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
  const Packet* GetVertexData() {
    return data_.data();
  }

  /**
   *  Returns a read-only pointer to the underlying index data.
   */ 
  const unsigned int* GetIndexData() {
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

 private:
  // the underlying data stored.
  std::vector<Packet> data_;

  // list of indices representing polygons formed from our vertices. must be triangles
  std::vector<unsigned int> indices_;

  // context used to make opengl calls
  std::unique_ptr<::screenspacemanager::model::VertexDataContext<Packet>> context_;

  // tracks whether we need to update our buffers.
  bool dirty_;

  
};

};  // namespace storage
};  // namespace screenspacemanager

#endif  // MESH_H_ 
