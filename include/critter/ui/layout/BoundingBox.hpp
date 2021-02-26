#ifndef BOUNDING_BOX_H_
#define BOUNDING_BOX_H_

#include <cinttypes>
#include <cstdlib>

namespace monkeysworld {
namespace critter {
namespace ui {
namespace layout {

struct BoundingBox {
  float left;
  float right;
  float top;
  float bottom;
};

}
}
}
}

#endif