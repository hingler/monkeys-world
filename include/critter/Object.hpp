#ifndef OBJECT_H_
#define OBJECT_H_

#include <utils/IDGenerator.hpp>

#include <vector>

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
   */ 
  Object();

  /**
   *  Visitor for identifying components :)
   */ 
  virtual void Accept(Visitor& v) = 0;

  /**
   *  Sets up all attributes required to draw this object.
   */ 
  virtual void PrepareAttributes() = 0;

  /**
   *  Renders the material itself.
   */ 
  virtual void RenderMaterial() = 0;

  /**
   *  Minimal commands necessary to draw this object onto the screen.
   *  Pairs with PrepareAttributes.
   */ 
  virtual void Draw() = 0;

  /**
   *  Finds a child by ID.
   *  @param id - The ID of the desired child.
   *  @returns Pointer to child, or NULL if invalid.
   */ 
  virtual Object* GetChild(uint64_t id) = 0;

  /**
   *  Returns a list of all children.
   */ 
  virtual std::vector<std::weak_ptr<Object>> GetChildren() = 0;

  /**
   *  Returns ptr to the parent object.
   */ 
  virtual Object* GetParent() = 0;

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

  // tba: noop v. dtor to ensure proper behavior



 private:
  uint64_t id_;
  static utils::IDGenerator id_generator_;
};

}
}

#endif  // OBJECT_H_