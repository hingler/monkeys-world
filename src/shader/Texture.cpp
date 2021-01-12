#include <shader/Texture.hpp>
#include <shader/exception/InvalidTexturePathException.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <boost/log/trivial.hpp>

#include <GLFW/glfw3.h>

namespace monkeysworld {
namespace shader {

Texture::Texture(const std::string& path) {
  // use stb image to load texture
  stbi_set_flip_vertically_on_load(true);
  tex_cache_ = stbi_load(path.c_str(), &width_, &height_, &channels_, 0);
  if (!tex_cache_) {
    BOOST_LOG_TRIVIAL(warning) << "could not load texture!";
    throw exception::InvalidTexturePathException("could not load texture");
  }
  tex_ = 0;
}

// find some way to pass the data type in on load
GLuint Texture::GetTextureDescriptor() const {
  if (tex_ == 0) {
    GLuint* tex = const_cast<GLuint*>(&tex_);
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, tex_);
    switch (channels_) {
      case 1:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_UNSIGNED_BYTE, tex_cache_);
        break;
      case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_cache_);
        break;
      case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_cache_);
        break;
      default:
        BOOST_LOG_TRIVIAL(error) << "not sure how to load this one tbh";
        break;
    }

    // tba: if it becomes necessary, preserve the texture cache, and use it to copy ctor.
    // in that case: make it a shared ptr and a RAII class.
    stbi_image_free(tex_cache_);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  return tex_;
}

uint64_t Texture::GetTextureSize() const {
  return width_ * height_ * channels_;
}

// risky if this isn't done on the main thread -- i dont think this'll be a problem but
Texture::~Texture() {
  if (tex_ != 0 && glfwGetCurrentContext()) {
    glDeleteTextures(1, &tex_);
  } else if (!glfwGetCurrentContext()) {
    BOOST_LOG_TRIVIAL(warning) << "Texture descriptor could not be destroyed!";
  }

  if (tex_cache_ != nullptr) {
    stbi_image_free(tex_cache_);
  }
}

}
}