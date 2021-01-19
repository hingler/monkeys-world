#ifndef OBJECT_H_
#define OBJECT_H_

#include <utils/IDGenerator.hpp>

#include <engine/RenderContext.hpp>

#include <engine/Context.hpp>

#include <vector>

// TBA: create forward decl headers in respective namespaces?

namespace monkeysworld {
namespace critter {

class Visitor;

/**
 *  Superclass for all objects which occupy space in the scene.
 *  Basic routines for nesting, revealing children,
 *  per-frame updates.
 * 
 *  Additionally: the context should be passed in here, instead of elsewhere.
 * 
 *  https://stackoverflow.com/questions/672373/can-i-call-a-base-classs-virtual-function-if-im-overriding-it/672376
 *  in case i forget: how to call base class method lol
 */ 
class Object { 
 public:
  /**
   *  Super ctor for objects
   *  @param ctx - pointer to context.
   */ 
  Object(engine::Context* ctx);

  /**
   *  Visitor for identifying components :)
   */ 
  virtual void Accept(Visitor& v) = 0;

  /**
   *  Sets up all attributes required to draw this object.
   *  DEPRECEATED -- moving all of this into RenderMaterial from now on >:)
   */ 
  virtual void PrepareAttributes() = 0;

  /**
   *  Renders the material itself.
   *  @param rc - Render context containing important scene information.
   */ 
  virtual void RenderMaterial(const engine::RenderContext& rc) = 0;

  /**
   *  Minimal commands necessary to draw this object onto the screen.
   *  Pairs with PrepareAttributes.
   * 
   *  Should be called by RenderMaterial, as well as other methods which intend to display info on screen.
   */ 
  virtual void Draw() = 0;

  /**
   *  Finds a child by ID.
   *  @param id - The ID of the desired child.
   *  @returns Pointer to child, or NULL if invalid.
   */ 
  virtual std::shared_ptr<Object> GetChild(uint64_t id) = 0;

  /**
   *  Returns a list of all children.
   */ 
  virtual std::vector<std::shared_ptr<Object>> GetChildren() = 0;

  /**
   *  Returns ptr to the parent object.
   */ 
  virtual std::shared_ptr<Object> GetParent() = 0;

  engine::Context* GetContext() const;

  /**
   *  Get the ID currently associated with an object.
   * 
   *  @returns ID.
   */ 
  uint64_t GetId();

  /**
   *  Changes the ID associated with this view.
   * 
   *  For now, does not freak out if duplicate IDs are generated.
   *  Assigned IDs might conflict with default IDs, keep the custom value
   *  sufficiently high to avoid clashes.
   */ 
  void SetId(uint64_t new_id);

  // UPDATE FUNCTIONS

  /**
   *  Update function called once per frame. Implementors are expected to override this.
   */ 
  virtual void Update();

  /**
   *  Destruction function called before leaving the current context.
   */ 
  virtual void Destroy();

  Object(const Object& other);
  Object(Object&& other);
  Object& operator=(const Object& other);
  Object& operator=(Object&& other);

  // nop -- just there so that we get dtor behavior in all subclasses
  virtual ~Object() {};

  // tba: noop v. dtor to ensure proper behavior



 private:
  engine::Context* ctx_;
  uint64_t id_;
  static utils::IDGenerator id_generator_;
};

}
}

#endif  // OBJECT_H_