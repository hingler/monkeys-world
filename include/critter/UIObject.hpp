#ifndef UI_OBJECT_H_
#define UI_OBJECT_H_

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <critter/GameObject.hpp>

#include <atomic>

namespace monkeysworld {
namespace critter {

/**
 *  UI Objects are objects which display on top of the rendered window, typically as 2D components.
 * 
 *  UI objects are grouped together, and are positioned relative to their containers.
 *  
 */ 
class UIObject : public Object, public std::enable_shared_from_this<UIObject> {
 public:
  
  UIObject(engine::Context* ctx);
  void Accept(Visitor& v) override;

  /**
   *  Returns a nullptr in all cases.
   */ 
  std::shared_ptr<Object> GetChild(uint64_t id) override;

  /**
   *  Returns an empty vector in all cases.
   */ 
  std::vector<std::weak_ptr<Object>> GetChildren() override;

  /**
   *  Returns parent of this component.
   */ 
  std::shared_ptr<Object> GetParent() override;

  /**
   *  Accepts incoming click events, and dispatches click events to children.
   *  TODO: Create "MouseEvent" to pass in as params to this and OnClick.
   * 
   *  @returns true if this UIObject's OnClick method consumed the event,
   *           false otherwise.
   */ 
  bool HandleClickEvent();

  /**
   *  Called whenever a click falls in this UI object's screen region.
   *  Overridden by inheriting clients.
   *  @returns true if this UIObject consumes the event, preventing it from
   *           passing further down the UI hierarchy.
   *           if false, event continues down.
   */
  virtual bool OnClick() { return false; };

  /**
   *  Returns the position of this object relative to its parent, in XY coords.
   */ 
  glm::vec2 GetPosition() const;

  /**
   *  Sets the position of this UI object.
   *  @param pos - the X/Y coords for this UIObject, relative to parent.
   */ 
  void SetPosition(glm::vec2 pos);

  /**
   *  @returns the XY size of this UI object.
   */ 
  glm::vec2 GetDimensions() const;

  /**
   *  Sets the size of this UI object.
   *  @param size - the size of the UIObject, in pixels wide/tall.
   */ 
  void SetDimensions(glm::vec2 size);

  // nop on this (deprecated)
  void PrepareAttributes() override {}

  /**
   *  Binds the framebuffer associated with this UI object and draws it to the screen.
   *  Calls DrawUI if the UI element has been invalidated by another call.
   */ 
  void RenderMaterial(const engine::RenderContext& rc) override;

  // also deprecated
  void Draw() override {}

  /**
   *  Defined by implementors.
   *  
   *  Prior to calling DrawUI, this UIObject will bind its associated framebuffer.
   *  The implementor then defines DrawUI to fill this framebuffer, before its parent
   *  group will then use the framebuffer's color attachment to draw the component to the screen.
   * 
   *  The framebuffer's dimensions can be accessed with "GetDimensions."
   *    Note that draw calls to the framebuffer will use clip space coordinates,
   *    so it is the implementor's responsibility to use commands which take this into account,
   *    or to define functionality which converts it to a more convenient clip space,
   *    for instance by using the resolution to transform components in-shader.
   */ 
  virtual void DrawUI() = 0;

  /**
   *  Invalidates the framebuffer, notifying the UIObject
   *  to redraw the UI component on the next render pass.
   */ 
  void Invalidate();

  UIObject(const UIObject& other);
  UIObject& operator=(const UIObject& other);
  UIObject(UIObject&& other);
  UIObject& operator=(UIObject&& other);
  ~UIObject();

 protected:
  /**
   *  @returns the descriptor for the color attachment to our UI framebuffer.
   */ 
  GLuint GetFramebufferColor();

  /**
   *  @returns true if the UIObject is still valid, false otherwise.
   */ 
  bool IsValid();

 private:
  glm::vec2 pos_;                                       // offset of this component relative to parent
  glm::vec2 size_;                                      // size of ui object, pixels wide/tall
  std::atomic_bool valid_;                              // whether or not the view has been invalidated.

  std::weak_ptr<UIObject> parent_;                      // parent object if valid

  GLuint framebuffer_;
  GLuint color_attach_;
  GLuint depth_stencil_;

  glm::vec2 fb_size_;                                   // last framebuffer size
};

}
}

#endif