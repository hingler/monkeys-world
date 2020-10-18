// gl lib provides binding utilities for components to use (ex. vertexdata uses a binding util)
// to abstract all GL calls, the idea being that we can test it more efficiently + abstract gl calls

#ifndef VERTEX_DATA_CONTEXT_GL_H_
#define VERTEX_DATA_CONTEXT_GL_H_

#include <vector>

#include <glad/glad.h>

#include <model/VertexDataContext.hpp>
#include <boost/log/trivial.hpp>

namespace screenspacemanager {
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
    glGenBuffers(1, &array_buffer_);
    glGenBuffers(1, &element_buffer_);
    glGenVertexArrays(1, &vao_);
    // vao now maps to attribute type
  }

  /**
   *  Populates the array+element buffers, as well as pointing the state machine at their contents.
   *  @param data - the vertex data being populated.
   *  @param indices - the associated indices.
   */ 
  void UpdateBuffersAndPoint(const std::vector<Packet>& data, const std::vector<unsigned int>& indices) override {
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer_);
    BOOST_LOG_TRIVIAL(debug) << "Adding " << data.size() << " vertices";
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Packet) * data.size(),
                 data.data(),
                 GL_STATIC_DRAW);

    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    BOOST_LOG_TRIVIAL(debug) << "Adding " << indices.size() << " indices";
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(unsigned int) * indices.size(),
                reinterpret_cast<const void*>(indices.data()),
                 GL_STATIC_DRAW);

    Packet::Bind();

  }

  /**
   *  Points the state machine at the array + element buffers.
   */ 
  void Point() override {
    glBindVertexArray(vao_);
    // buffers are bound, data has not changed
  }

 private:
  GLuint array_buffer_;
  GLuint element_buffer_;
  GLuint vao_;
};

};  // namespace opengl
};  // namespace screenspacemanager

#endif  // VERTEX_DATA_CONTEXT_GL_H_