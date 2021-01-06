#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <utils/IDGenerator.hpp>

#include <engine/Context.hpp>
#include <critter/Object.hpp>
#include <critter/Camera.hpp>

#include <glm/glm.hpp>

#include <atomic>
#include <cinttypes>
#include <memory>
#include <vector>

namespace monkeysworld {
namespace critter {

/**
 *  The GameObject class represents a drawable, 3D object which can be represented
 *  onscreen.
 * 
 *  Along with the instructions necessary to draw itself, this object also interfaces with
 *  other objects, providing resources for detecting collisions*, local 3D transformations*,
 *  and more*
 * 
 *  TODO: implement object visitor for identifying components
 * 
 */ 
class GameObject : public Object, public std::enable_shared_from_this<GameObject> {
 public:

  GameObject(engine::Context* ctx);

  void Accept(Visitor& v) override;

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
  std::shared_ptr<Object> GetChild(uint64_t id) override;

  std::vector<std::weak_ptr<Object>> GetChildren() override;

  /**
   *  Returns ptr to the object associated with this object.
   */ 
  std::shared_ptr<Object> GetParent() override;

  /**
   *  Returns ptr to context.
   */ 
  engine::Context* GetContext() const;

  /**
   *  Returns the transformation matrix associated with this object.
   */ 
  glm::mat4 GetTransformationMatrix() const;

  /**
   *  Sets XYZ position.
   */ 
  void SetPosition(const glm::vec3& new_pos);

  /**
   *  Sets XYZ rotation.
   */ 
  void SetRotation(const glm::vec3& new_rot);

  /**
   *  Sets XYZ scale.
   */ 
  void SetScale(const glm::vec3& new_scale);

  /**
   *  Returns a pointer to the currently active camera.
   */ 
  virtual std::shared_ptr<Camera> GetActiveCamera();

  // getters for the above.
  const glm::vec3& GetPosition() const;
  const glm::vec3& GetRotation() const;
  const glm::vec3& GetScale() const;

  // note: moves DO NOT preserve parent child relationship! this must be restored.
  GameObject(const GameObject& other);
  GameObject(GameObject&& other);
  GameObject& operator=(const GameObject& other);
  GameObject& operator=(GameObject&& other);

  // copy + move ctors are necessary lol

 protected:
  /**
   *  Remove a directly nested child
   */ 
  void RemoveChild(uint64_t id);

  /**
   *  0-arg ctor for tests only.
   */ 
  GameObject();

 private:
  glm::vec3 position;
  glm::vec3 rotation;
  glm::vec3 scale;

  std::weak_ptr<GameObject> parent_;
  // These fields could become contentious if we're going multi-thread.
  // if that ends up being the case: lock it here!
  // alt: probably only one thread at a time will attempt to alter these fields.
  // don't worry about it :)

  // set of all children associated with this object
  std::vector<std::shared_ptr<GameObject>> children_;

  // cached transformation matrix
  glm::mat4 tf_matrix_cache_;

  // whether or not the transformation matrix on store is safe.
  std::atomic_bool dirty_;

  engine::Context* ctx_;

  // TODO: is a lock on transformation calls necessary, or will access be limited to a single thread?
};

} // namespace critter
} // namespace monkeysworld

#endif  // GAME_OBJECT_H_