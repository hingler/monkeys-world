#include <shader/Framebuffer.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <boost/log/trivial.hpp>

#include <algorithm>

namespace monkeysworld {
namespace shader {

Framebuffer::Framebuffer() {
  fb_ = color_ = depth_stencil_ = 0;
  fb_size_ = glm::ivec2(0);
  fb_size_old_ = glm::ivec2(1);
}

void Framebuffer::SetDimensions(glm::ivec2 size) {
  if (size.x <= 0 || size.y <= 0) {
    return;
  }

  fb_size_ = size;
}

glm::ivec2 Framebuffer::GetDimensions() const {
  return fb_size_;
}

void Framebuffer::BindFramebuffer() {
  BindFramebuffer(FramebufferTarget::DEFAULT);
}

void Framebuffer::BindFramebuffer(FramebufferTarget target) {
  GLenum targ;
  switch (target) {
    case FramebufferTarget::DEFAULT:
      targ = GL_FRAMEBUFFER;
      break;
    case FramebufferTarget::DRAW:
      targ = GL_DRAW_FRAMEBUFFER;
      break;
    case FramebufferTarget::READ:
      targ = GL_READ_FRAMEBUFFER;
      break;
  }

  if (fb_size_ != fb_size_old_) {
    GenerateFramebuffer(targ);
  }

  glBindFramebuffer(targ, fb_);
}

GLuint Framebuffer::GetColorAttachment() {
  return color_;
}

// deprecate soon
GLuint Framebuffer::GetFramebuffer() {
  return fb_;
}

void Framebuffer::GenerateFramebuffer(GLenum target) {
  glDeleteFramebuffers(1, &fb_);
  glDeleteTextures(1, &color_);
  glDeleteTextures(1, &depth_stencil_);

  glGenTextures(1, &color_);
  glGenTextures(1, &depth_stencil_);
  glBindTexture(GL_TEXTURE_2D, depth_stencil_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,
                static_cast<uint32_t>(fb_size_.x), static_cast<uint32_t>(fb_size_.y),
                0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
  glBindTexture(GL_TEXTURE_2D, color_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                static_cast<uint32_t>(fb_size_.x), static_cast<uint32_t>(fb_size_.y),
                0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  glGenFramebuffers(1, &fb_);
  glBindFramebuffer(target, fb_);
  glFramebufferTexture2D(target, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_, 0);
  glFramebufferTexture2D(target, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_stencil_, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  if (glCheckFramebufferStatus(target) != GL_FRAMEBUFFER_COMPLETE) {
    BOOST_LOG_TRIVIAL(error) << "incomplete framebuffer :(";
  } 

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  fb_size_old_ = fb_size_;
}

Framebuffer::~Framebuffer() {
  if (!glfwGetCurrentContext()) {
    BOOST_LOG_TRIVIAL(error) << "could not delete GL components!";
  } else if (fb_ != 0) {
    glDeleteFramebuffers(1, &fb_);
    glDeleteTextures(1, &color_);
    glDeleteTextures(1, &depth_stencil_);
  }
}

Framebuffer::Framebuffer(const Framebuffer& other) {
  fb_ = color_ = depth_stencil_ = 0;
  fb_size_ = other.fb_size_;
  fb_size_old_ = glm::ivec2(0, 0);
}

Framebuffer& Framebuffer::operator=(const Framebuffer& other) {
  fb_size_ = other.fb_size_;
  return *this;
}

Framebuffer::Framebuffer(Framebuffer&& other) {
  fb_ = other.fb_;
  color_ = other.color_;
  depth_stencil_ = other.depth_stencil_;
  other.fb_ = other.color_ = other.depth_stencil_ = 0;
  fb_size_ = other.fb_size_;
  fb_size_old_ = other.fb_size_old_;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) {
  fb_ = other.fb_;
  color_ = other.color_;
  depth_stencil_ = other.depth_stencil_;
  other.fb_ = other.color_ = other.depth_stencil_ = 0;
  fb_size_ = other.fb_size_;
  fb_size_old_ = other.fb_size_old_;
  return *this;
}

}
}