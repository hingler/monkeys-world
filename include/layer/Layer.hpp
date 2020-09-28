
#ifndef LAYER_H_
#define LAYER_H_

#include "utils/IDGenerator.hpp"

#include <glad/glad.h>
#include <string>
#include <cinttypes>

namespace screenspacemanager {
namespace layer {

struct Point {
  int x;
  int y;
};

class Layer {
 public:
  /**
   *  Create a new independent layer instance. Initializes dimensions at 128x128.
   */ 
  Layer();

  /**
   * Creates a new independent layer instance with the provided dimensions.
   */ 
  Layer(int width, int height);

  /**
   *  Set the size of this layer.
   */ 
  void SetDimensions(int width, int height);
  void SetDimensions(Point dims);

  /**
   *  Set the position of this layer.
   */ 
  void SetPosition(int x, int y);
  void SetPosition(Point coords);

  /**
   *  Get the dimensions.
   */ 
  const Point* GetDimensions();

  /**
   *  Get the position.
   */ 
  const Point* GetPosition();

  /**
   *  Binds the internal framebuffer and calls Render
   */ 
  void BindFramebufferAndRender();

  /**
   *  Renders the layer onto the internal framebuffer.
   *  Must be overwritten by class implementation.
   */ 
  virtual void Render() = 0;

  /**
   *  Returns the unique identifier assigned to this layer.
   */ 
  uint64_t GetLayerId();

  /**
   *  Changes the layer id to a passed in string
   */ 
  void SetLayerId(int id);

  /**
   *  Returns a layer whose ID matches the passed parameter.
   */ 
  virtual std::shared_ptr<Layer> FindLayerById(int id) = 0;

  /**
   *  Returns the framebuffer associated with this layer.
   */ 
  GLuint GetFramebufferColor();

  /**
   *  Erases the canvas and lets you start from scratch :)
   */ 
  void Clear();

 private:

  // Recreates all textures associated with framebuffer (on resize, for instance)
  void GenerateFramebufferTextures();

  GLuint framebuffer_;                  // descriptor for frame buffer
  GLuint framebuffer_color_;            // descriptor for color component of frame buffer
  GLuint framebuffer_depth_stencil_;    // descriptor for depth + stencil of frame buffer

  Point dims_;                          // dimensions
  Point coords_;                        // x/y coordinates.

  static IDGenerator id_generator_;     // used to generate unique IDs when created.

  uint64_t id_;                      // Identifier associated with this layer.

 protected:
  /**
   *  Compiles a shader program from vertex and fragment shader paths.
   *  For utility -- we'll almost definitely need this all over the place.
   */ 
  static bool CompileProgram(std::string vertex_path, std::string fragment_path, GLuint* output);

  /**
   *  Compiles a single shader from a file.
   */ 
  static bool CompileShader(std::string shader_path, GLuint type, GLuint* output);
};

};  // namespace layer
};  // namespace screenspacemanager


#endif