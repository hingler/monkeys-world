#ifndef VERTEX_DATA_H_
#define VERTEX_DATA_H_

#include "glad/glad.h"
#include <algorithm>
#include <vector>

// from https://stackoverflow.com/questions/87372/check-if-a-class-has-a-member-function-of-a-given-signature/10707822#10707822

/**
 *  Represents the data associated with the vertices which comprise a model.
 *  @tparam Packet - the specification for the vertex data which will be passed in.
 *                   this encompasses data types, and locations.
 */ 
template <typename Packet>
class VertexData {
 private:
  template <typename T>
  struct HasBind {
    template <typename U, typename void (*)()> struct Tester {};
    template <typename U> static char Test(Tester<U, &U::Bind> *);
    template <typename U> static int Test(...);

    // true if `Bind` method is available, false otherwise
    static constexpr bool Has = (sizeof(Test<T>(NULL)) == sizeof(char));
  };

  static_assert(HasBind<Packet>::Has, "Bind method not available for templated class");
 public:
  /**
   *  Constructs a new VertexData object.
   */ 
  VertexData() : dirty_(true) {
    glGenBuffers(2, &vertex_buffer_);
    glGenVertexArrays(1, &vao_);
  }

  /**
   *  Stores a new vertex based on the contents of the data packet.
   *  @param packet - data packet containing vertex data.
   */ 
  void AddVertex(const T& packet) {
    vertex_buffer_.push_back(packet);
  }

  /**
   *  Stores a new triangle based on the described indices.
   *  @param vertA - First vertex
   *  @param vertB - Second vertex
   *  @param vertC - third vertex
   * 
   *  @return true if the vertices are in-bounds, and false otherwise.
   */ 
  bool AddPolygon(int vertA, int vertB, int vertC) {
    std::initializer_list<int> vert_list({vertA, vertB, vertC});
    int min_value = std::min(vert_list);
    int max_value = std::max(vert_list);
    if (min_value > 0 && max_value < getVertexCount()) {
      indices_.push_back(vertA);
      indices_.push_back(vertB);
      indices_.push_back(vertC);
      return true;
    }

    return false;
  }

  /**
   *  Returns number of vertices stored here.
   */ 
  size_t getVertexCount() const {
    return data_.size();
  }

  /**
   *  Returns number of polygons stored in indices.
   */ 
  size_t getPolyCount() const {
    return indices_.size() / 3;
  }

  /**
   *  Binds the vertex data to the GL context based on template specification.
   */ 
  void Bind() {
    // bind buffers and vao
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBindVertexArray(vao_);
    // if dirty, repopulate and call T bind

    if (dirty_) {
      glBufferData(GL_ARRAY_BUFFER, vertex_buffer_.size() * sizeof(Packet), vertex_buffer_.data, GL_STATIC_DRAW);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_.size() * sizeof(int), index_buffer_.data, GL_STATIC_DRAW);
      T::Bind();

      dirty_ = false;
    }
  }

  ~VertexData() {
    glDeleteBuffers(2, &vertex_buffer_);
    glDeleteVertexArrays(1, &vao_);
  }

  /**
   *  Copy ctor
   */ 
  VertexData(const VertexData& other) : dirty_(true) {
    data_ = other.data_;
    indices_ = other.indices_;

    // recreate the data fields in case we intend to modify
    glGenBuffers(2, &vertex_buffer_);
    glGenVertexArrays(1, &vao_);
  }

  /**
   *  Move ctor
   */ 
  VertexData(VertexData&& other) :
    data_(std::move(other.data_)),
    indices_(std::move(other.indices_)),
    vertex_buffer_(other.vertex_buffer_),
    index_buffer_(other.index_buffer_),
    vao_(other.vao_),
    dirty_(other.dirty_) {
  }

  /**
   *  Assign copy
   */ 
  VertexData& operator=(const VertexData& other);

  /**
   *  Assign move
   */ 
  VertexData& operator=(VertexData&& other) {
    data_ = std::move(other.data_);
    indices_ = std::move(other.indices_);
    vertex_buffer_ = other.vertex_buffer_;
    index_buffer_ = other.index_buffer_;
    vao_ = other.vao_;
    dirty_ = other.dirty_;
  }


 private:
  // the underlying data stored.
  std::vector<T> data_;

  // list of indices representing polygons formed from our vertices. must be triangles
  std::vector<int> indices_;

  // gl buffer for vertices
  GLuint vertex_buffer_;

  // gl buffer for indices
  GLuint index_buffer_;

  // stores attribute bindings
  GLuint vao_;

  // true if the buffer data needs to be updated, false if not.
  bool dirty_;
};

#endif  // VERTEX_DATA_H_