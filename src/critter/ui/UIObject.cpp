#include <critter/ui/UIObject.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {

using shader::materials::TextureXferMaterial;

std::weak_ptr<shader::materials::TextureXferMaterial> UIObject::xfer_mat_singleton_;
std::mutex UIObject::xfer_lock_;
model::Mesh<storage::VertexPacket2D> UIObject::xfer_mesh_;

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

std::vector<std::shared_ptr<Object>> UIObject::GetChildren() {
  return std::vector<std::shared_ptr<Object>>();
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
    if (auto i = std::static_pointer_cast<UIObject>(child)) {
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

glm::vec2 UIObject::GetAbsolutePosition() const {
  if (auto i = parent_.lock()) {
    return i->GetAbsolutePosition() + GetPosition();
  }

  return GetPosition();
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, size_.x, size_.y, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glBindTexture(GL_TEXTURE_2D, color_attach_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size_.x, size_.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glGenFramebuffers(1, &framebuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attach_, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_stencil_, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      BOOST_LOG_TRIVIAL(error) << "incomplete ui framebuffer :(";
    } 

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
      auto ui = std::static_pointer_cast<UIObject>(child);
      ui->RenderMaterial(rc);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
    glViewport(0, 0, fb_size_.x, fb_size_.y);
    
    #ifdef DEBUG
      glClearColor(1.0, 0.0, 0.0, 0.2);
    #else
      glClearColor(0.0, 0.0, 0.0, 0.0);
    #endif
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    DrawUI(min, max);
    valid_.store(true);
  }
}

void UIObject::DrawToScreen() {
  // create a mesh
  std::unique_lock<std::mutex> lock(xfer_lock_);
  if (!xfer_mat_) {
    if (!(xfer_mat_ = xfer_mat_singleton_.lock())) {
      xfer_mat_ = std::make_shared<TextureXferMaterial>(GetContext());
      xfer_mat_singleton_ = xfer_mat_;
    }
  }

  auto pos = GetAbsolutePosition();
  auto dim = GetDimensions();


  storage::VertexPacket2D temp;
  if (xfer_mesh_.GetVertexCount() == 0) {
    // this will be our sign that the mesh has been prepared
    for (int i = 0; i < 4; i++) {
      temp.texcoords.x = (i >= 2 ? 1 : 0);
      temp.texcoords.y = (i > 0 && i < 3 ? 0 : 1);
      xfer_mesh_.AddVertex(temp);
    }

    xfer_mesh_.AddPolygon(0, 1, 2);
    xfer_mesh_.AddPolygon(0, 2, 3);
  }

  glm::ivec2 win;
  GetContext()->GetFramebufferSize(&win.x, &win.y);

  // top left
  xfer_mesh_[0].position.x = (pos.x / win.x) * 2 - 1;
  xfer_mesh_[0].position.y = 1 - (pos.y / win.y) * 2;

  // bottom left
  xfer_mesh_[1].position.x = (pos.x / win.x) * 2 - 1;
  xfer_mesh_[1].position.y = 1 - ((pos.y + dim.y) / win.y) * 2;

  // bottom right
  xfer_mesh_[2].position.x = ((pos.x + dim.x) / win.x) * 2 - 1;
  xfer_mesh_[2].position.y = 1 - ((pos.y + dim.y) / win.y) * 2;
  
  // top right
  xfer_mesh_[3].position.x = ((pos.x + dim.x) / win.x) * 2 - 1;
  xfer_mesh_[3].position.y = 1 - (pos.y / win.y) * 2;

  xfer_mesh_.PointToVertexAttribs();
  xfer_mat_->SetTexture(color_attach_);
  xfer_mat_->UseMaterial();
  glDrawElements(GL_TRIANGLES, xfer_mesh_.GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
}

GLuint UIObject::GetFramebufferColor() {
  return color_attach_;
}

bool UIObject::IsValid() {
  if (valid_) {
    for (auto child : GetChildren()) {
      auto ui = std::static_pointer_cast<UIObject>(child);
      if (!ui->IsValid()) {
        return false;
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
      auto ui = std::static_pointer_cast<UIObject>(child);
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

UIObject::UIObject(const UIObject& other) : Object(other) {
  pos_ = other.pos_;
  size_ = other.size_;
  fb_size_ = glm::vec2(0, 0);

  valid_ = false;
  parent_ = std::weak_ptr<UIObject>();
  
  framebuffer_ = 0;
  color_attach_ = 0;
  depth_stencil_ = 0;
}

UIObject& UIObject::operator=(const UIObject& other) {
  Object::operator=(other);
  pos_ = other.pos_;
  size_ = other.size_;
  valid_ = false;
  return *this;
}

UIObject::UIObject(UIObject&& other) : Object(other) {
  pos_ = std::move(other.pos_);
  size_ = std::move(other.size_);
  fb_size_ = std::move(other.fb_size_);

  valid_ = other.valid_.load();
  parent_ = std::weak_ptr<UIObject>();

  framebuffer_ = other.framebuffer_;
  color_attach_ = other.color_attach_;
  depth_stencil_ = other.depth_stencil_;

  other.framebuffer_ = other.color_attach_ = other.depth_stencil_ = 0;
}

UIObject& UIObject::operator=(UIObject&& other) {
  Object::operator=(other);
  pos_ = std::move(other.pos_);
  size_ = std::move(other.size_);
  fb_size_ = std::move(other.fb_size_);

  valid_ = other.valid_.load();
  parent_ = std::weak_ptr<UIObject>();

  framebuffer_ = other.framebuffer_;
  color_attach_ = other.color_attach_;
  depth_stencil_ = other.depth_stencil_;

  other.framebuffer_ = other.color_attach_ = other.depth_stencil_ = 0;
  return *this;
}

UIObject::~UIObject() {
  if (glfwGetCurrentContext()) {
    if (framebuffer_ != 0) {
      glDeleteFramebuffers(1, &framebuffer_);
      glDeleteTextures(1, &color_attach_);
      glDeleteTextures(1, &depth_stencil_);
    }
  }
}

}
}
}