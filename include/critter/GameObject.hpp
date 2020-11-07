#ifndef OBJECT_H_
#define OBJECT_H_

#include <glm/glm.hpp>

#include <cinttypes>
#include <memory>
#include <unordered_set>

namespace monkeysworld {
namespace critter {

/**
 *  The GameObject class represents a drawable, 3D object which can be represented
 *  onscreen.
 * 
 *  Along with the instructions necessary to draw itself, this object also interfaces with
 *  other objects, providing resources for detecting collisions*, local 3D transformations*,
 *  and more*
 */ 
class GameObject {
 public:
  /**
   *  Makes all GL calls necessary to draw the game object on screen.
   */ 
  virtual void Draw() = 0;

  /**
   *  Adds a child to this GameObject.
   *  @param child - shared pointer to the new child.
   */ 
  void AddChild(std::shared_ptr<GameObject> child);

  /**
   *  Find child by id.
   *  @param id - the ID of the desired child.
   *  @returns Pointer to child, or NULL if invalid.
   */ 
  GameObject* GetChild(uint64_t id);

  /**
   *  Returns ptr to the object associated with this object.
   */ 
  GameObject* GetParent();

  /**
   *  Get the ID currently associated with an object
   * 
   *  @returns ID, if not valid. 0 is reserved for an invalid ID.
   */ 
  uint64_t GetId();

  // go with this for now!
  // later: we'll probably have some methods which simplify transformations
  // there should be a private method which actually compiles the transformation
  // from these fields
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;

 protected:
  /**
   *  Remove a directly nested child
   */ 
  void RemoveChild(uint64_t id);

 private:
  GameObject* parent_;
  // These fields could become contentious if we're going multi-thread.
  // if that ends up being the case: lock it here!
  // alt: probably only one thread at a time will attempt to alter these fields.
  // don't worry about it :)
  std::unordered_set<std::shared_ptr<GameObject>> children_;
  // static ID generator, probably, since these need to be unique
};

} // namespace critter
} // namespace monkeysworld

#endif  // OBJECT_H_