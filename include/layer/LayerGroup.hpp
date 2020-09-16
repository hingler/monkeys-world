#ifndef LAYER_GROUP_H_
#define LAYER_GROUP_H_

#include "layer/Layer.hpp"
#include <memory>

namespace screenspacemanager {
namespace layer {
/**
 *  Layer groups contain several layers
 */ 
class LayerGroup : Layer {
 public:
  /**
   *  Add a layer to this layer group. This group assumes ownership of the layer.
   */ 
  void AddLayer(std::shared_ptr<Layer> layer);

  /**
   *  Removes a child layer based on a pointer to another layer.
   */ 
  void RemoveChild(std::shared_ptr<Layer> layer);


  /**
   *  Removes a child layer based on a layer's ID which appears somewhere in the hierarchy.
   */ 
  void RemoveChild(std::string layerId);
};

};  // namespace layer
};  // namespace screenspacemanager

#endif