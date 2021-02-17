#ifndef FRAMEBUFFER_H_
#define FRAMEBUFFER_H_

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace monkeysworld {
namespace shader {

/**
 *  Specifies the target which this framebuffer is bound to.
 *  Used by BindFramebuffer.
 */ 
enum class FramebufferTarget {
  DRAW,
  READ,
  DEFAULT
};

/**
 *  A wrapper which represents a framebuffer and its texture attachments.
 */ 
class Framebuffer {
 public:
  /**
   *  Constructs a new Framebuffer object.
   *  Size is initialized to (1, 1).
   */ 
  Framebuffer();

  /**
   *  Modifies the dimensions of the framebuffer.
   *  @param size - the new width and height of the framebuffer.
   *                Behavior is undefined if either dimension is less than 1.
   */ 
  void SetDimensions(glm::ivec2 size);

  /**
   *  Returns the dimensions of this framebuffer.
   */ 
  glm::ivec2 GetDimensions() const;

  /**
   *  Equivalent to BindFramebuffer(FramebufferTarget::DEFAULT).
   */ 
  void BindFramebuffer();

  /**
   *  Binds this framebuffer to a specified target.
   *  @param target - The desired target to bind this framebuffer to.
   *                    - DRAW - binds this framebuffer to only the draw target.
   *                    - READ - binds this framebuffer to only the read target.
   *                    - DEFAULT - binds this framebuffer to both the draw and read targets.
   */ 
  void BindFramebuffer(FramebufferTarget target);

  /**
   *  @returns color attachment?
   */ 
  GLuint GetColorAttachment();

  /**
   *  @returns framebuffer identifier.
   */ 
  GLuint GetFramebuffer();

  ~Framebuffer();
  Framebuffer(const Framebuffer& other);
  Framebuffer& operator=(const Framebuffer& other);
  Framebuffer(Framebuffer&& other);
  Framebuffer& operator=(Framebuffer&& other);

 private:
  GLuint fb_;
  GLuint color_;
  GLuint depth_stencil_;

  glm::ivec2 fb_size_;
  glm::ivec2 fb_size_old_;
};

}
}

#endif