#ifndef TEXTURE_H_
#define TEXTURE_H_

#include <glad/glad.h>
#include <shader/Framebuffer.hpp>

#include <memory>
#include <string>

namespace monkeysworld {

namespace engine {
class Context;
}

namespace shader {

/**
 *  Represents a texture.
 *  Textures do nothing to manage state -- the client should expect that
 *  texture state is not saved between calls, and that someone else might modify
 *  this texture object.
 * 
 *  TODO: as soon as i can, fix the texture thingy.
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
   *  @param channels - number of channels in the new texture.
   */ 
  Texture(int width, int height, int channels);

  /**
   *  Creates a new texture from the contents of a framebuffer.
   *  @param ctx - the currently active context.
   *  @param fb - the framebuffer being copied. Always grabs GL_COLOR_ATTACHMENT0.
   */ 
  Texture(engine::Context* ctx, std::shared_ptr<Framebuffer> fb);

  /**
   *  @returns the descriptor associated with this texture.
   */ 
  GLuint GetTextureDescriptor() const;

  int GetWidth() const {
    return width_;
  }

  int GetHeight() const {
    return height_;
  }

  int GetChannelCount() const {
    return channels_;
  }

  /**
   *  Returns the size of this texture, in bytes.
   *  Returns 0 if the size is unknown.
   */ 
  uint64_t GetTextureSize() const;

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
  // TODO: these ought to be const and public
  int width_;
  int height_;
  int channels_;
};

}
}

#endif