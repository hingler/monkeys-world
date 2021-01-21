#ifndef TEXT_FORMAT_H_
#define TEXT_FORMAT_H_

namespace monkeysworld {
namespace font {

/**
 *  Controls the horizontal alignment of characters within a line
 */ 
enum AlignmentH {
  LEFT,   // first character will fall on [0, 0] (DEFAULT)
  RIGHT,  // last character will end on [0, 0]
  CENTER  // centers characters around origin
};

/**
 *  Controls the vertical alignment of text lines between one another
 */ 
enum AlignmentV {
  TOP,        // the highest character will touch the x-intercept
  MIDDLE,     // center lines around origin
  BOTTOM,     // the bottom of the lowest line will touch the x-intercept
  DEFAULT     // the first line rests on the baseline, the rest trail below
};

/**
 *  Several formatting options pertaining to text.
 */ 
struct TextFormat {
  AlignmentH horiz_align;
  AlignmentV vert_align;
  float char_spacing;       // space, in 1/64 pt, between chars.
};

}
}

#endif