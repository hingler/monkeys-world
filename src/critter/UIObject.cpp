#include <critter/UIObject.hpp>

namespace monkeysworld {
namespace critter {

UIObject::UIObject(engine::Context* ctx) : Object(ctx) {
  pos_ = glm::vec2(0, 0);
  size_ = glm::vec2(0, 0);
  fb_size_ = glm::vec2(0, 0);

  valid_ = true;
  parent_ = std::weak_ptr<UIObject>();

  framebuffer_ = 0;
  color_attach_ = 0;
  depth_stencil_ = 0;
}

void UIObject::Accept(Visitor& v) {
  // nop for now -- need to write the actual visitor :)
}

std::shared_ptr<Object> UIObject::GetChild(uint64_t id) {
  return std::shared_ptr<Object>(nullptr);
}

std::vector<std::weak_ptr<Object>> UIObject::GetChildren() {
  return std::vector<std::weak_ptr<Object>>();
}

std::shared_ptr<Object> UIObject::GetParent() {
  return parent_.lock();
}

bool UIObject::HandleClickEvent() {
  bool event = OnClick();
  if (event) {
    return true;
  }

  for (auto child : GetChildren()) {
    if (auto i = std::static_pointer_cast<UIObject>(child.lock())) {
      if (i->HandleClickEvent()) {
        return true;
      }
    }
  }

  return false;
}

glm::vec2 UIObject::GetPosition() const {
  return pos_;
}

void UIObject::SetPosition(glm::vec2 pos) {
  pos_ = pos;
}

glm::vec2 UIObject::GetDimensions() const {
  return size_;
}

void UIObject::SetDimensions(glm::vec2 size) {
  size_ = size;
  // new size requires a redraw
  Invalidate();
}

void UIObject::Invalidate() {
  valid_.store(false);
}

void UIObject::RenderMaterial(const engine::RenderContext& rc) {
  // get invalid bounding box
  if (fb_size_ != size_) {
    if (framebuffer_ != 0) {
      // out with the old
      glDeleteFramebuffers(1, &framebuffer_);
      glDeleteTextures(1, &color_attach_);
      glDeleteTextures(1, &depth_stencil_);
    }

    // in with the new
    glGenTextures(1, &color_attach_);
    glGenTextures(1, &depth_stencil_);
    glBindTexture(GL_TEXTURE_2D, depth_stencil_);
    glTexStorage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, size_.x, size_.y);
    glBindTexture(GL_TEXTURE_2D, color_attach_);
    glTexStorage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_.x, size_.y);

    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attach_, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_stencil_, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // update framebuffer size
    fb_size_ = size_;
  }

  if (!IsValid()) {
    // at least one child is invalid
    // get invalid binding box (while invalidity still applies)
    glm::vec2 min, max;
    GetInvalidatedBoundingBox(&min, &max);
    
    // render all children first (bottom up rendering)
    for (auto child : GetChildren()) {
      if (auto c = child.lock()) {
        auto ui = std::static_pointer_cast<UIObject>(c);
        ui->RenderMaterial(rc);
      }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glViewport(0, 0, fb_size_.x, fb_size_.y);
    DrawUI(min, max);
    valid_.store(true);
  }
}

GLuint UIObject::GetFramebufferColor() {
  return color_attach_;
}

bool UIObject::IsValid() {
  if (valid_) {
    for (auto child : GetChildren()) {
      if (auto c = child.lock()) {
        auto ui = std::static_pointer_cast<UIObject>(c);
        if (!ui->IsValid()) {
          return false;
        }
      }
    }

    // all children are still valid
    return true;
  }

  // this uiobject is invalid regardless
  return false;
}

void UIObject::GetInvalidatedBoundingBox(glm::vec2* xyMin, glm::vec2* xyMax) {
  if (!valid_) {
    // invalidate entire screen
    *xyMin = glm::vec2(0);
    *xyMax = GetDimensions();
    return;
  } else {
    glm::vec2 min;
    glm::vec2 max;
    bool set = false;
    for (auto child : GetChildren()) {
      if (auto c = child.lock()) {
        auto ui = std::static_pointer_cast<UIObject>(c);
        if (!ui->IsValid()) {
          // the first child defines the region
          if (!set) {
            set = true;
            min = ui->GetPosition();
            max = min + ui->GetDimensions();
          } else {
            min = glm::min(min, ui->GetPosition());
            max = glm::max(max, ui->GetDimensions() + ui->GetPosition());
          }
        }
      }
    }

    if (!set) {
      // all children were valid -- no need to draw
      *xyMin = glm::vec2(0);
      *xyMax = glm::vec2(0);
      return;
    }

    *xyMin = min;
    *xyMax = max;
    return;
  }
}

}
}