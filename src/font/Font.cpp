#include <font/Font.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace font {

using model::Mesh;
using storage::VertexPacket2D;

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
  e = FT_Set_Char_Size(face_, 0, bitmap_desired_scale * 64, 72, 72);

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
  
  atlas_width = width_px;
  atlas_height = height_px;

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
      temp_info.valid = false;
      continue;
    }

    // check bitmap format
    if (temp_glyph->format != FT_GLYPH_FORMAT_BITMAP) {
      FT_Render_Glyph(temp_glyph, FT_RENDER_MODE_NORMAL);
    }

    temp_info.valid = true;
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

    temp_info.origin_x = ((float)width_px / atlas_width);

    glyph_cache_[i - glyph_lower_] = temp_info;

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

// advance is stored in 1/64 pixels
#define ADVANCE_SCALE 64.0f

model::Mesh<storage::VertexPacket2D> Font::GetTextGeometry(const std::string& text, float size_pt) {
  // scales our fonts down to screenspace scale (roughly:)
  const float SCREENSPACE_FAC = (960.0f * bitmap_desired_scale) / size_pt;
  
  Mesh<VertexPacket2D> result;
  float origin_x = -0.8f;
  // bitmap, bearing are in pixels
  // advance is in 1/64 pixels.

  // origin of glyph, in object space
  float glyph_origin_x;
  float glyph_origin_y;

  // texture width and height
  float tex_width;
  float tex_height;

  // geometry width and height
  float geom_width;
  float geom_height;

  glyph_info* info;

  int cur = 0;
  for (auto c : text) {
    if (c < glyph_lower_ || c > glyph_upper_) {
      // skip, make space
      origin_x += glyph_cache_[0].advance / (SCREENSPACE_FAC * ADVANCE_SCALE);
      continue;
    }

    info = &glyph_cache_[c - glyph_lower_];
    glyph_origin_x = origin_x + (info->bearing_x / SCREENSPACE_FAC);
    glyph_origin_y = 0.0f + (info->bearing_y / SCREENSPACE_FAC);

    tex_width = ((float)info->width) / atlas_width;
    tex_height = ((float)info->height) / atlas_height;

    geom_width = info->width / SCREENSPACE_FAC;
    geom_height = info->height / SCREENSPACE_FAC;

    result.AddVertex({glm::vec2(glyph_origin_x, glyph_origin_y),                            glm::vec2(info->origin_x, 0.0f)});
    result.AddVertex({glm::vec2(glyph_origin_x, glyph_origin_y - geom_height),              glm::vec2(info->origin_x, tex_height)});
    result.AddVertex({glm::vec2(glyph_origin_x + geom_width, glyph_origin_y - geom_height), glm::vec2(info->origin_x + tex_width, tex_height)});
    result.AddVertex({glm::vec2(glyph_origin_x + geom_width, glyph_origin_y),               glm::vec2(info->origin_x + tex_width, 0.0f)});
    result.AddPolygon(cur * 4, cur * 4 + 1, cur * 4 + 2);
    result.AddPolygon(cur * 4 + 2, cur * 4 + 3, cur * 4);

    cur++;

    origin_x += (info->advance / (SCREENSPACE_FAC * ADVANCE_SCALE));
  }

  return result;
}

GLuint Font::GetGlyphAtlas() {
  return glyph_texture_;
}

}
}