#ifndef CANVAS_LAYER_H_
#define CANVAS_LAYER_H_

#include "Font.hpp"
#include "Paint.hpp"
#include "Layer.hpp"

/**
 *  Type of layer which is specialized for drawing content onto the screen.
 */ 
class Layer2D : public Layer {

  // DRAW COMMNADS

  /**
   *  Draw a circle onto this layer.
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

};

#endif