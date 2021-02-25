#ifndef MARGIN_H_
#define MARGIN_H_

#include <cinttypes>

namespace monkeysworld {
namespace critter {
namespace ui {
namespace layout {

enum class Face {
  TOP,
  BOTTOM,
  LEFT,
  RIGHT,
  IGNORE
};

/**
 *  Represents a relationship between the UIObject containing the margin,
 *  and some other UIObject.
 */ 
struct Margin {

  // the ID of the object which this object is anchored to.
  uint64_t anchor_id;

  // the face of the anchor to attach to.
  Face anchor_face;

  // the distance, in px, between this object and the anchor.
  float dist;
};

}
}
}
}

#endif