#ifndef OBJECT_H_
#define OBJECT_H_

#include <critter/Context.hpp>
#include <utils/IDGenerator.hpp>

namespace monkeysworld {
namespace critter {

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
  Object(Context* ctx);

  /**
   *  Sets up all attributes required to draw this object.
   */ 
  virtual void PrepareAttributes() = 0;

  /**
   *  Renders the material itself.
   */ 
  virtual void RenderMaterial() = 0;

  /**
   *  Finds a child by ID.
   *  @param id - The ID of the desired child.
   *  @returns Pointer to child, or NULL if invalid.
   */ 
  virtual Object* GetChild(uint64_t id) = 0;

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
   *  Returns ptr to the currently active context.
   */ 
  Context* GetContext();

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



 private:
    // id generator for new game objects
  Context* ctx_;
  uint64_t id_;
  static utils::IDGenerator id_generator_;
};

}
}

#endif  // OBJECT_H_