#ifndef LAYER_2D_H_
#define LAYER_2D_H_

#include "Font.hpp"
#include "Paint.hpp"
#include "layer/Layer.hpp"

#include <vector>

namespace screenspacemanager {
namespace layer {
  
/**
 *  Type of layer which is specialized for drawing content onto the screen.
 */ 
class Layer2D : public Layer {

  // DRAW COMMNADS
 public:

  /**
   *  Create a new Layer2D -- 128 x 128.
   */ 
  Layer2D();

  /**
   *  Create a new Layer2D with predetermined dimensions.
   */ 
  Layer2D(int width, int height);

  
  std::shared_ptr<Layer> FindLayerById(int id) override;

  /**
   *  Draw a circle onto this layer.
   *  All of these methods assume that the frame buffer has already been bound.
   *  @param x - the x coordinate of the circle's center.
   *  @param y - the y coordinate of the circle's center.
   *  @param radius - the radius of the circle.
   *  @param brush - the Paint object used to draw the circle.
   */ 
  void DrawCircle(int x, int y, int radius, Paint brush);

  /**
   *  Draw a rectangle onto this layer.
   *  @param x - the x coordinate of the top left corner of the rectangle.
   *  @param y - the y coordinate of the top left corner of the rectangle.
   *  @param width - the width of the rectangle.
   *  @param height - the height of the rectangle.
   *  @param brush - the Paint object used to draw the rectangle.
   */ 
  void DrawRect(int x, int y, int width, int height, Paint brush);

  /**
   *  Draws text onto the screen.
   *  @param x - the x coordinate of the text anchor.
   *  @param y - the y coordinate of the text anchor.
   *  @param font - the font being drawn onto the screen, as well keyning, alignment, etc.
   *  @param brush - The paint object used to draw the text.
   */ 
  void DrawText(int x, int y, Font font, Paint brush);

 private:
  std::vector<float> GenerateBoundingBox(float xMin, float yMin, float xMax, float yMax, std::vector<uint32_t>* indices);

};
};  // namespace layer
};  // namespace screenspacemanager

#endif