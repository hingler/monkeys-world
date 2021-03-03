#ifndef CANVAS_H_
#define CANVAS_H_

#include <shader/Framebuffer.hpp>
#include <model/Mesh.hpp>
#include <storage/VertexPacketTypes.hpp>

#include <shader/Texture.hpp>
#include <shader/FilterSequence.hpp>

#include <memory>

namespace monkeysworld {
namespace shader {

/**
 *  The Canvas object is a wrapper around a framebuffer
 *  which enables the client to draw some basic primitives onto the framebuffer.
 * 
 *  Since the framebuffer is still accessible by other means, one is still able to write custom
 *  drawing code -- however, this canvas object attempts to simplify some common operations.
 */ 
class Canvas {
 public:
  /**
   *  Creates a new canvas.
   *  @param framebuffer -- the framebuffer which is now associated with this canvas.
   */ 
  Canvas(std::shared_ptr<Framebuffer> framebuffer);

// TODO: replace "color" with some notion of a more complex fill? i.e. gradients, etc.

  /**
   *  Draws a line onto the canvas.
   *  @param start -- start point of the line, in pixels.
   *  @param end -- end point of the line, in pixels.
   *  @param thickness -- thickness of the line.
   *  @param color -- color of the line.
   */ 
  void DrawLine(glm::vec2 start, glm::vec2 end, float thickness, glm::vec4 color);
  
  /**
   *  Draws an image onto the screen, with no filters.
   *  @param tex - the texture being drawn onto the screen.
   *  @param origin - position of top left corner of image.
   */
  void DrawImage(std::shared_ptr<const Texture> tex, glm::vec2 origin, glm::vec2 dims);

  /**
   *  Draws an image onto the screen, with filters.
   *  @param tex - the texture being drawn onto the screen.
   *  @param origin - position of top left corner of image.
   *  @param filter - the sequence of filters used to draw the image.
   */ 
  void DrawImage(std::shared_ptr<const Texture> tex, glm::vec2 origin, glm::vec2 dims, const FilterSequence& filter);
 private:
  // sets up the mesh to render an image. acquire geom lock before using
  void SetupImageMesh(std::shared_ptr<const Texture>& tex, glm::vec2 origin, glm::vec2 dims);
  std::shared_ptr<Framebuffer> fb_;
  
};

}
}

#endif