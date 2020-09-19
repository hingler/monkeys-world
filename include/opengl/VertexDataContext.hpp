#ifndef VERTEX_DATA_CONTEXT_H_
#define VERTEX_DATA_CONTEXT_H_

namespace screenspacemanager {
namespace opengl {

/**
 *  Interface used for the VertexDataContext.
 */ 
template <typename Packet>
class VertexDataContext {
 public:
  virtual void PopulateBuffersAndBind(const std::vector<Packet>& data, const std::vector<int>& indices) = 0;
};

};  // namespace opengl
};  // namespace screenspacemanager

#endif