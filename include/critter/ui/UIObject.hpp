#ifndef UI_OBJECT_H_
#define UI_OBJECT_H_

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <critter/GameObject.hpp>

#include <shader/materials/TextureXferMaterial.hpp>

#include <model/Mesh.hpp>

#include <atomic>
#include <mutex>

namespace monkeysworld {
namespace critter {
namespace ui {

class UIGroup;

/**
 *  UI Objects are objects which display on top of the rendered window, typically as 2D components.
 * 
 *  UI objects are grouped together, and are positioned relative to their containers.
 *  
 */ 
class UIObject : public Object, public std::enable_shared_from_this<UIObject> {
  // quick hack lol
  friend class UIGroup;
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
  std::vector<std::shared_ptr<Object>> GetChildren() override;

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
   *  @returns the absolute position, in xy coords, of this object, from top left.
   */ 
  glm::vec2 GetAbsolutePosition() const;

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

  /**
   *  Draws the contents of this framebuffer directly to the screen.
   */ 
  void DrawToScreen();

  // also deprecated
  void Draw() override {}

  /**
   *  Defined by implementors.
   *  
   *  Prior to calling DrawUI, this UIObject will bind its associated framebuffer.
   *  The implementor then defines DrawUI to fill this framebuffer, before its parent
   *  group will then use the framebuffer's color attachment to draw the component to the screen.
   * 
   *  This function will only be called if a UIObject has been invalidated.
   * 
   *  The framebuffer's dimensions can be accessed with "GetDimensions."
   *    Note that draw calls to the framebuffer will use clip space coordinates,
   *    so it is the implementor's responsibility to use commands which take this into account,
   *    or to define functionality which converts it to a more convenient clip space,
   *    for instance by using the resolution to transform components in-shader.
   *  
   *  The parameters to this method represent the minimally invalidated bounding box
   *  for this component -- implementors can use these args to save performance
   *  by only updating a part of the screen, instead of the whole thing.
   * 
   *  @param minXY, the minXY of the invalid bounding box, origin top left.
   *  @param maxXY, the maxXY of the invalid bounding box, origin top left.
   */ 
  virtual void DrawUI(glm::vec2 minXY, glm::vec2 maxXY) = 0;

  /**
   *  Invalidates the framebuffer, notifying the UIObject
   *  to redraw the UI component on the next render pass.
   */ 
  void Invalidate();

  /**
   *  @returns the descriptor for the color attachment to our UI framebuffer.
   */ 
  GLuint GetFramebufferColor();

  UIObject(const UIObject& other);
  UIObject& operator=(const UIObject& other);
  UIObject(UIObject&& other);
  UIObject& operator=(UIObject&& other);
  ~UIObject();

 protected:

  /**
   *  @returns true if the UIObject is still valid, false otherwise.
   */ 
  bool IsValid();

 private:
  std::weak_ptr<UIObject> parent_;                      // parent object if valid
  glm::vec2 pos_;                                       // offset of this component relative to parent
  glm::vec2 size_;                                      // size of ui object, pixels wide/tall
  std::atomic_bool valid_;                              // whether or not the view has been invalidated.


  GLuint framebuffer_;
  GLuint color_attach_;
  GLuint depth_stencil_;

  glm::vec2 fb_size_;                                   // last framebuffer size

  // for rendering directly to screen
  // this is typically only done by a single UI object, so all fields are static.
  // however, they technically could be used to draw other components to screen as well.
  static std::weak_ptr<shader::materials::TextureXferMaterial> xfer_mat_singleton_; 
  static std::mutex xfer_lock_;
  static model::Mesh<storage::VertexPacket2D> xfer_mesh_;

  // handling drawing to screen
  // we should probably cache the mesh, instead of recreating it on each call
  // (i won't deal with it yet though >:])
  // uiobjects would have a mesh which contains their screen draw data
  // this mesh would generally be very small, but could be used
  // (in fact most of the time it would just be empty)
  // alternatively...
  // we could allocate a static mesh, and let all instances share it
  // only one instance draws at a time, and we just contractually lock it

  std::shared_ptr<shader::materials::TextureXferMaterial> xfer_mat_;

  // weak ptr for shader (shader will only ever run on a single thread -- no problem!)
  // UIObject will contain a method which draws the UIObject directly to the screen,
  // bypassing the hierarchy altogether.

  // plus: ideally, only one element (the top level group) will need it at a time

  /**
   *  Calculates the minimal bounding box which needs to be updated.
   *  @param xyMin - output parameter for min XY
   *  @param xyMax - output parameter for max XY
   */ 
  void GetInvalidatedBoundingBox(glm::vec2* xyMin, glm::vec2* xyMax);
};

}
}
}

#endif