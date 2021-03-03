#ifndef MARGIN_SIZE_H_
#define MARGIN_SIZE_H_

namespace monkeysworld {
namespace critter {
namespace ui {
namespace layout {

enum class MarginType {
  AUTO,     // fit self between two anchors -- ignores bounds if both margins on axis are `auto`
  DEFAULT   // some distance from anchor
};

class MarginSize {
  
 public:

  MarginSize() {
    this->type = MarginType::DEFAULT;
    this->dist = 0;
  }

  /**
   *  Creates a new MarginSize with the passed type.
   *  Size defaults to 0.
   */ 
  MarginSize(MarginType type) {
    this->type = type;
    this->dist = 0;
  }

  /**
   *  Creates a new DEFAULT margin with the specified distance.
   */ 
  MarginSize(float dist) {
    this->type = MarginType::DEFAULT;
    this->dist = dist;
  }

  /**
   *  Return dist when casting to float implicitly
   */ 
  operator float() const {
    return dist;
  }
  
  // type of margin
  MarginType type;

  // distance from anchor
  float dist;
};

}
}
}
}

#endif