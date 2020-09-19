// gl lib provides binding utilities for components to use (ex. vertexdata uses a binding util)
// to abstract all GL calls, the idea being that we can test it more efficiently + abstract gl calls

#ifndef VERTEX_DATA_CONTEXT_GL_H_
#define VERTEX_DATA_CONTEXT_GL_H_

#include <vector>

#include <glad/glad.h>

#include "opengl/VertexDataContext.hpp"

namespace screenspacemanager {
namespace opengl {

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
    glGenBuffers(2, &array_buffer_);
    glGenVertexArrays(1, &vao_);
  }

  /**
   *  Populate a passed-in array buffer and element buffer, then point to their attributes.
   *  @param data - the vertex data being populated.
   *  @param indices - the associated indices.
   */ 
  void PopulateBuffersAndBind(const std::vector<Packet>& data, const std::vector<int>& indices) override {
    glBindBuffer(array_buffer_, GL_ARRAY_BUFFER);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Packet) * data.size(),
                 reinterpret_cast<const void*>(data.data()),
                 GL_STATIC_DRAW);
    
    glBindBuffer(element_buffer_, GL_ELEMENT_ARRAY_BUFFER);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(int) * indices.size(),
                 reinterpret_cast<const void*>(indices.data()),
                 GL_STATIC_DRAW);

    glBindVertexArray(vao_);

    // should only do this for ctor
    Packet::Bind();
  }

 private:
  GLuint array_buffer_;
  GLuint element_buffer_;
  GLuint vao_;
};

};  // namespace opengl
};  // namespace screenspacemanager

#endif  // VERTEX_DATA_CONTEXT_GL_H_