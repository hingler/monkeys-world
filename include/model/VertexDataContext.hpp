#ifndef VERTEX_DATA_CONTEXT_H_
#define VERTEX_DATA_CONTEXT_H_

namespace monkeysworld {
namespace model {

enum VertexDataContextType {
  gl,
  debug
};

/**
 *  Interface used for the VertexDataContext.
 */ 
template <typename Packet>
class VertexDataContext {
 public:

  /**
   *  Populates the GL buffers managed by this context, then points to their appropriate attributes.
   *  @param data - The vertex attribute data.
   *  @param indices - triplets of indices representing triangles.
   */ 
  virtual void UpdateBuffersAndPoint(const std::vector<Packet>& data, const std::vector<unsigned int>& indices) = 0;

  /**
   *  If no attributes have been modified, then we can simply bind the VAO.
   */ 
  virtual void Point() = 0;

  /**
   *  Returns an enum representing the type of this context.
   */ 
  virtual VertexDataContextType GetType() = 0;

  virtual ~VertexDataContext() {}
};

}  // namespace opengl
}  // namespace monkeysworld

#endif