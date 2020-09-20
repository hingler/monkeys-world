#ifndef VERTEX_DATA_CONTEXT_H_
#define VERTEX_DATA_CONTEXT_H_

namespace screenspacemanager {
namespace model {

/**
 *  Interface used for the VertexDataContext.
 */ 
template <typename Packet>
class VertexDataContext {
 public:

  /**
   *  
   */ 
  virtual void PopulateBuffersAndPoint(const std::vector<Packet>& data, const std::vector<int>& indices) = 0;
};

};  // namespace opengl
};  // namespace screenspacemanager

#endif