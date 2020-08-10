#include "layer/Layer.hpp"
#include "Paint.hpp"

Layer::Layer() : Layer(128, 128) { }

Layer::Layer(int width, int height) {
  // create framebuffer
  // thats it really
  glGenFramebuffers(1, &framebuffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

  dims_ = {width, height};

  glGenTextures(2, &framebuffer_color_);    // should be contiguous
  GenerateFramebufferTextures();

  id_ = id_generator_.CreateUniqueID();

  // generate some random ID which can be used here
  // 

}

void Layer::SetDimensions(Point dims) {
  SetDimensions(dims.x, dims.y);
}

void Layer::SetDimensions(int width, int height) {
  dims_.x = width;
  dims_.y = height;
}

void Layer::SetPosition(Point coords) {
  SetPosition(coords.x, coords.y);
}

void Layer::SetPosition(int x, int y) {
  coords_.x = x;
  coords_.y = y;
}

const Point* Layer::GetDimensions() {
  return &dims_;
}

const Point* Layer::GetPosition() {
  return &coords_;
}

void Layer::BindFramebufferAndRender() {
  // todo: handle invalidated views
  glViewport(0, 0, dims_.x, dims_.y);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
}

int Layer::GetLayerId() {
  return id_;
}

void Layer::SetLayerId(int id) {
  id_generator_.InsertUniqueID(id);
  id_ = id;
}

GLuint Layer::GetFramebufferColor() {

  return framebuffer_color_;
}

// layer count should stay small so a vector is probably fine?
// query each sub layer for the ID and see if it's there with a simple equality check
// if a layer's ID chances we lose if in the hashing method
// groups will check their children
// leaves will return themselves if the passed ID matches theirs.

void Layer::GenerateFramebufferTextures() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, framebuffer_color_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dims_.x, dims_.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebuffer_color_, 0);

  glBindTexture(GL_TEXTURE_2D, framebuffer_depth_stencil_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, dims_.x, dims_.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebuffer_depth_stencil_, 0);
}