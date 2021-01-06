#include <font/Font.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace font {

std::mutex Font::ft_lib_lock_;
std::weak_ptr<FTLibWrapper> Font::lib_singleton_;

// this scheme requires us to lock on calls to new_face and done_face
// as well as when we check if the weak_ptr is valid, in case two fonts
// attempt to create the lib at the same time.
Font::Font(const std::string& font_path) {
  FT_Error e;

  {
    std::lock_guard<std::mutex> lock(ft_lib_lock_);
    if (!(ft_lib_ = lib_singleton_.lock())) {
      ft_lib_ = std::make_shared<FTLibWrapper>();
      lib_singleton_ = ft_lib_;
    }

    e = FT_New_Face(ft_lib_->lib, font_path.c_str(), 0, &face_);
    BOOST_LOG_TRIVIAL(trace) << font_path;
    if (e) {
      // complain some more :/
      BOOST_LOG_TRIVIAL(error) << "Could not create new face";
      exit(EXIT_FAILURE);
    }

  }

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // what should char size be?
  // let's go with 256px for now
  e = FT_Set_Char_Size(face_, 0, 256 * 64, 72, 72);

  // create an array of glyphs we can use from here on out

  glyph_cache_ = new glyph_info[glyph_upper_ - glyph_lower_ + 1];

  // figure out amount of space needed for chars
  // wrap, and 1px of space on either side to avoid artefacts of storage method


  unsigned int width_px  = 0;
  unsigned int height_px = 0;
  FT_GlyphSlot temp_glyph = nullptr;
  FT_UInt glyph_index;
  for (char i = glyph_lower_; i <= glyph_upper_; i++) {
    glyph_index = FT_Get_Char_Index(face_, i);
    e = FT_Load_Glyph(face_, glyph_index, FT_LOAD_DEFAULT);
    if (e) {
      BOOST_LOG_TRIVIAL(warning) << "Could not load char " << i << " -- skipping...";
      continue;
    }

    temp_glyph = face_->glyph;
    height_px = std::max(height_px, temp_glyph->bitmap.rows);
    width_px += (temp_glyph->bitmap.width + 1);
  }

  // ensure that vertical wrap is not a problem
  height_px++;

  BOOST_LOG_TRIVIAL(trace) << "tex size: " << width_px << " x " << height_px;

  glActiveTexture(GL_TEXTURE0);
  // generate the texture
  glGenTextures(1, &glyph_texture_);
  glBindTexture(GL_TEXTURE_2D, glyph_texture_);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_px, height_px, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
  // go through the font list again, this time drawing on to the screen

  width_px = 0;

  glyph_info temp_info;

  for (char i = glyph_lower_; i <= glyph_upper_; i++) {
    glyph_index = FT_Get_Char_Index(face_, i);
    e = FT_Load_Glyph(face_, glyph_index, FT_LOAD_RENDER);
    if (e) {
      // should have already been handled.
      continue;
    }

    // check bitmap format
    if (temp_glyph->format != FT_GLYPH_FORMAT_BITMAP) {
      FT_Render_Glyph(temp_glyph, FT_RENDER_MODE_NORMAL);
    }

    temp_glyph = face_->glyph;
    glTexSubImage2D(GL_TEXTURE_2D, 
                    0,
                    width_px,
                    0,
                    temp_glyph->bitmap.width,
                    temp_glyph->bitmap.rows,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    temp_glyph->bitmap.buffer);

    // add glyph data to the glyph info
    temp_info.advance = temp_glyph->advance.x;

    temp_info.bearing_x = temp_glyph->bitmap_left;
    temp_info.bearing_y = temp_glyph->bitmap_top;

    temp_info.width = temp_glyph->bitmap.width;
    temp_info.height = temp_glyph->bitmap.rows;

    temp_info.origin_x = width_px;

    width_px += (temp_glyph->bitmap.width + 1);
  }
  
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  /**
   * TODO: can we be done with the face after ctor?
   * If so: we still want to keep the lib shared ptr
   * because it saves us lib constructions, the point is solely
   * to ensure that we have a lib to start, and destroy it before we're done.
   */
}

GLuint Font::GetGlyphAtlas() {
  return glyph_texture_;
}

}
}