// gl lib provides binding utilities for components to use (ex. vertexdata uses a binding util)
// to abstract all GL calls, the idea being that we can test it more efficiently + abstract gl calls

#ifndef VERTEX_DATA_CONTEXT_GL_H_
#define VERTEX_DATA_CONTEXT_GL_H_

#include <vector>

#include <glad/glad.h>

#include <model/VertexDataContext.hpp>
#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace model {

/**
 *  Provides a rendering context for the VertexData class, allowing it to make calls to GL.
 */ 
template <typename Packet>
class VertexDataContextGL : public VertexDataContext<Packet> {
  
 public:
  /**
   *  Creates a new VertexDataContext.
   */ 
  VertexDataContextGL() {
    gl_alloced_ = false;
    index_buffer_size_ = 0;
    array_buffer_size_ = 0;
  }

  /**
   *  Populates the array+element buffers, as well as pointing the state machine at their contents.
   *  @param data - the vertex data being populated.
   *  @param indices - the associated indices.
   */ 
  void UpdateBuffersAndPoint(const std::vector<Packet>& data, const std::vector<unsigned int>& indices) const override {
    if (!gl_alloced_) {
      glGenBuffers(1, const_cast<GLuint*>(&array_buffer_));
      glGenBuffers(1, const_cast<GLuint*>(&element_buffer_));
      glGenVertexArrays(1, const_cast<GLuint*>(&vao_));
      bool* alloced_nonconst_ = const_cast<bool*>(&gl_alloced_);
      *alloced_nonconst_ = true;
    }

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
    uint64_t ab_size = sizeof(Packet) * data.size();

    if (ab_size > array_buffer_size_) {
      glBufferData(GL_ARRAY_BUFFER,
                  sizeof(Packet) * data.size(),
                  NULL,
                  GL_DYNAMIC_DRAW);
      // orphan 
      glBufferSubData(GL_ARRAY_BUFFER,
                      0,
                      ab_size,
                      data.data());
      uint64_t* ab_size_nonconst_ = const_cast<uint64_t*>(&array_buffer_size_);
      *ab_size_nonconst_ = ab_size;
      BOOST_LOG_TRIVIAL(trace) << "buffer " << array_buffer_ << " reallocated";
    } else {
      glBufferSubData(GL_ARRAY_BUFFER,
                      0,
                      ab_size,
                      data.data());
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    uint64_t ib_size = sizeof(unsigned int) * indices.size();

    if (ib_size > index_buffer_size_) {
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                  sizeof(unsigned int) * indices.size(),
                  NULL,
                  GL_DYNAMIC_DRAW);
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    0,
                    ib_size,
                    indices.data());
      uint64_t* ib_size_nonconst_ = const_cast<uint64_t*>(&index_buffer_size_);
      *ib_size_nonconst_ = ib_size;
      BOOST_LOG_TRIVIAL(trace) << "index buffer " << element_buffer_ << " reallocated";
    } else {
      // https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
      // some suggestion of recreating buffers in this case...
      // http://hacksoflife.blogspot.com/2015/06/glmapbuffer-no-longer-cool.html
      glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
                    0,
                    ib_size,
                    indices.data());
    }
    

    Packet::Bind();

  }

  /**
   *  Points the state machine at the array + element buffers.
   */ 
  void Point() const override {
    glBindVertexArray(vao_);
    // buffers are bound, data has not changed
  }

  VertexDataContextType GetType() const override {
    return VertexDataContextType::gl;
  }

  ~VertexDataContextGL() {
    if (gl_alloced_) {
      glDeleteBuffers(1, &array_buffer_);
      glDeleteBuffers(1, &element_buffer_);
      glDeleteVertexArrays(1, &vao_);
    }
  }

 private:
  GLuint array_buffer_;
  GLuint element_buffer_;
  GLuint vao_;

  // true if our buffers have been created -- false otherwise
  bool gl_alloced_;

  uint64_t array_buffer_size_;
  uint64_t index_buffer_size_;
};

};  // namespace opengl
};  // namespace monkeysworld

#endif  // VERTEX_DATA_CONTEXT_GL_H_