#include <shader/Texture.hpp>
#include <shader/exception/InvalidTexturePathException.hpp>

#include <engine/Context.hpp>

#include <boost/log/trivial.hpp>


#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

Texture::Texture(int width, int height, int channels) : width_(width),
                                                        height_(height),
                                                        channels_(channels),
                                                        tex_(0),
                                                        tex_cache_(nullptr) {}

Texture::Texture(engine::Context* ctx, std::shared_ptr<Framebuffer> fb) {
  auto dims = fb->GetDimensions();
  width_ = dims.x;
  height_ = dims.y;
  channels_ = 4;

  auto exec_prog = [&, fb] {
    glGenTextures(1, &tex_);
    glBindTexture(GL_TEXTURE_2D, tex_);
    glTexStorage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_);
    // fb->BindFramebuffer(FramebufferTarget::READ);
    // glReadBuffer(GL_COLOR_ATTACHMENT0);
    glCopyImageSubData(fb->GetColorAttachment(), GL_TEXTURE_2D, 0, 0, 0, 0, tex_, GL_TEXTURE_2D, 0, 0, 0, 0, width_, height_, 1);
    tex_cache_ = nullptr;
  };

  auto f = ctx->GetExecutor()->ScheduleOnMainThread(exec_prog);
  f.wait();
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

    // TODO: the best solution would seemingly be to have the loader load all these textures into its own cache,
    // then our textures will read them from bytes to a texture object.

    // I AM NOT GOING TO DO THIS RIGHT NOW. but i'll do it later :)
    if (tex_cache_) {
      stbi_image_free(tex_cache_);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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