#ifndef FILTER_SEQUENCE_H_
#define FILTER_SEQUENCE_H_

#include <memory>
#include <vector>

namespace monkeysworld {
namespace shader {

/**
 *  Stores filter types.
 */ 
enum class FilterName {
  HSL
};

/**
 *  Generic representation of a filter.
 */ 
struct filter_data {
  FilterName type;
};

/**
 *  HSL filter.
 */ 
struct filter_hsl : filter_data {
  float hue;
  float sat;
  float light;
};

/**
 *  A FilterSequence represents a list of filter operations to be performed by our shader.
 */ 
class FilterSequence {
 public:

  /**
   *  Creates a new FilterSequence object, initialized to an empty series of filter operations.
   */ 
  FilterSequence();

  /**
   *  Adds an HSL filter to this FilterSequence.
   *  @param hue - the number of degrees by which to hue-shift the corresponding image. (-inf, inf)
   *  @param sat - value to offset saturation of image by. (-1, 1).
   *  @param light - value to offset lightness of image by. (-1, 1).
   */ 
  void AddHSLFilter(float hue, float sat, float light);

  /**
   *  Returns the filter associated with a given index of our filter sequence.
   */ 
  std::shared_ptr<const filter_data> operator[](int index);

  /**
   *  @returns number of filters currently contained in this filter sequence.
   */ 
  int GetFilterCount();
 private:

  // stores ordered list of filters
  std::vector<std::shared_ptr<filter_data>> filters_;
};

}
}

#endif