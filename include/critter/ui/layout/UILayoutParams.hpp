#ifndef UI_LAYOUT_PARAMS_H_
#define UI_LAYOUT_PARAMS_H_

#include <critter/ui/layout/Margin.hpp>

namespace monkeysworld {
namespace critter {
namespace ui {
namespace layout {

/**
 *  A struct which defines how UIObjects should be laid out within a group.
 */ 
struct UILayoutParams {
  Margin top;
  Margin bottom;
  Margin left;
  Margin right;
};

}
}
}
}

#endif