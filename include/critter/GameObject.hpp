#ifndef OBJECT_H_
#define OBJECT_H_

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
  // make the gl calls necessary to draw the object onto the screen
  void Draw();
};

} // namespace critter
} // namespace monkeysworld

#endif  // OBJECT_H_