#include <font/Font.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace monkeysworld {
namespace font {

std::mutex Font::ft_lib_lock_;
std::weak_ptr<FTLibWrapper> Font::lib_singleton_;

Font::Font(const std::string& font_path) {
  {
    std::lock_guard<std::mutex> lock(ft_lib_lock_);
    if (!(ft_lib_ = lib_singleton_.lock())) {
      ft_lib_ = std::make_shared<FTLibWrapper>();
      lib_singleton_ = ft_lib_;
    }

     FT_Error e;
     e = FT_New_Face(ft_lib_->lib, font_path.c_str(), 0, &face_);
     if (e) {
       // complain some more :/
     }
  }

  // create an array of glyphs we can use from here on out
  /**
   * TODO: can we be done with the face after ctor?
   * If so: we still want to keep the lib shared ptr
   * because it saves us lib constructions, the point is solely
   * to ensure that we have a lib to start, and destroy it before we're done.
   */
}

}
}