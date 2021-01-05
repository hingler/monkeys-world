#ifndef FT_WRAPPER_H_
#define FT_WRAPPER_H_

#include <ft2build.h>
#include FT_FREETYPE_H

namespace monkeysworld {
namespace font {
class FTLibWrapper {
 public:
  FTLibWrapper() {
    FT_Error e;
    e = FT_Init_FreeType(&lib);
    if (e) {
      // complain or something idk
    }
  }
  ~FTLibWrapper() {
    FT_Error e;
    e = FT_Done_FreeType(lib);
    if (e) {
      // complain again i guess?
    }
  }
  
  FT_Library lib;
};
}
}

#endif