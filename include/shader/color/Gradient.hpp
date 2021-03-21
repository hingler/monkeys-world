#ifndef GRADIENT_H_
#define GRADIENT_H_

#include <glm/glm.hpp>

#include <set>
#include <vector>

namespace monkeysworld {
namespace shader {
namespace color {

/**
 *  Marks a gradient stop, a location where a color is defined.
 */ 
struct GradientStop {
  // color of this stop
  glm::vec4 col;

  // position of this stop along gradient.
  float stop;
};

/**
 *  Represents a gradient which can be drawn to the screen.
 */ 
class Gradient {
 public:
  /**
   *  Creates a new gradient. The gradient is initially empty.
   */ 
  Gradient();

  // start of gradient, px relative to top left.
  glm::vec2 start_point;

  // end of gradient, px relative to top left.
  glm::vec2 end_point;

  /**
   *  Adds a stop to the gradient.
   *  @param col - the color of this gradient stop.
   *  @param stop - the point along our gradient where we want this color [0, 1].
   */ 
  void AddStop(glm::vec4 col, float stop);

  /**
   *  Removes all stops from this object, 
   */ 
  void ClearStops();

  /**
   *  @returns an ordered list of gradient stops.
   *           if no colors have been specified yet, returns a single white stop at 0.5.
   */ 
  std::vector<GradientStop> GetStops();

 private:
  /**
   *  Compares gradients between one another.
   */ 
  struct GradientComparator {
    bool operator()(const GradientStop& a, const GradientStop& b) const {
      return (a.stop < b.stop);
    }
  };

  // stores gradient stops in order.
  std::set<GradientStop, GradientComparator> gradient_set_;
};

}
}
}

#endif