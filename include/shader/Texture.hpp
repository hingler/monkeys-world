#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <glad/glad.h>

#include <string>

namespace monkeysworld {
namespace shader {

// TODO: How would we load an image, and then write onto that image?
// 

/**
 *  Represents a texture.
 *  Textures do nothing to manage state -- the client should expect that
 *  texture state is not saved between calls, and that someone else might modify
 *  this texture object.
 */ 
class Texture {
 public:
  /**
   *  Creates a new texture object and loads an image into it.
   *  @param path - path to the desired texture.
   */ 
  Texture(const std::string& path);

  /**
   *  Creates a new empty texture object with given dimensions.
   *  @param width - width of the new texture.
   *  @param height - height of the new texture.
   */ 
  Texture(int width, int height);

  /**
   *  @returns the descriptor associated with this texture.
   */ 
  GLuint GetTextureDescriptor() const;

  ~Texture();
  Texture(const Texture& other) = delete;
  Texture& operator=(const Texture& other) = delete;
  Texture(Texture&& other) = delete;
  Texture& operator=(Texture&& other) = delete;
 private:
  // stores the texture before being loaded by GL.
  unsigned char* tex_cache_;
  GLuint tex_;
  // tex dims
  int width_;
  int height_;
  int channels_;
};

}
}

#endif