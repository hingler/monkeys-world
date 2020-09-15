#include "layer/Layer2D.hpp"

Layer2D::Layer2D() : Layer2D(128, 128) { }

Layer2D::Layer2D(int width, int height) : Layer(width, height) {
  
}

std::shared_ptr<Layer> Layer2D::FindLayerById(int id) {
  uint64_t my_id;
  if ((my_id = GetLayerId()) == id) {
    return std::shared_ptr<Layer>(this);
  }

  return nullptr;
}

void Layer2D::DrawCircle(int x, int y, int radius, Paint brush) {
  std::vector<uint32_t> indices;
}