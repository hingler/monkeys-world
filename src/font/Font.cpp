#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <font/Font.hpp>
#include <font/exception/BadFontPathException.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace font {

using model::Mesh;
using storage::VertexPacket2D;
using exception::BadFontPathException;

std::mutex Font::ft_lib_lock_;
std::weak_ptr<FTLibWrapper> Font::lib_singleton_;

// this scheme requires us to lock on calls to new_face and done_face
// as well as when we check if the weak_ptr is valid, in case two fonts
// attempt to create the lib at the same time.
Font::Font(const std::string& font_path) {
  if (!glfwGetCurrentContext()) {
    BOOST_LOG_TRIVIAL(warning) << "No context exists on this thread!";
  }

  glyph_texture_ = 0;
  FT_Error e;
  FT_Face face;

  {
    std::lock_guard<std::mutex> lock(ft_lib_lock_);
    if (!(ft_lib_ = lib_singleton_.lock())) {
      ft_lib_ = std::make_shared<FTLibWrapper>();
      lib_singleton_ = ft_lib_;
    }

    e = FT_New_Face(ft_lib_->lib, font_path.c_str(), 0, &face);
    BOOST_LOG_TRIVIAL(trace) << font_path;
    if (e) {
      // complain some more :/
      BOOST_LOG_TRIVIAL(error) << "Could not create new face";
      throw BadFontPathException("Could not create new face");
    }

  }

  ascent_ = face->size->metrics.ascender;

  // what should char size be?
  // let's go with 256px for now
  e = FT_Set_Char_Size(face, 0, bitmap_desired_scale * 64, 72, 72);
  
  line_height_ = face->size->metrics.height;

  // create an array of glyphs we can use from here on out

  glyph_cache_ = new glyph_info[glyph_upper_ - glyph_lower_ + 1];

  // figure out amount of space needed for chars
  // wrap, and 1px of space on either side to avoid artefacts of storage method


  unsigned int width_px  = 0;
  unsigned int height_px = 0;
  FT_GlyphSlot temp_glyph = nullptr;
  FT_UInt glyph_index;
  for (char i = glyph_lower_; i <= glyph_upper_; i++) {
    glyph_index = FT_Get_Char_Index(face, i);
    e = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    if (e) {
      BOOST_LOG_TRIVIAL(warning) << "Could not load char " << i << " -- skipping...";
      continue;
    }

    temp_glyph = face->glyph;
    height_px = std::max(height_px, temp_glyph->bitmap.rows);
    width_px += (temp_glyph->bitmap.width + 1);
  }

  // ensure that vertical wrap is not a problem
  height_px++;

  BOOST_LOG_TRIVIAL(trace) << "tex size: " << width_px << " x " << height_px;
  
  atlas_width = width_px;
  atlas_height = height_px;

  // create our in-memory store
  memory_cache_ = new char[atlas_width * atlas_height];

  BOOST_LOG_TRIVIAL(trace) << "allocated " << atlas_width * atlas_height << " bytes for cache";
  // go through the font list again, this time drawing on to the screen

  width_px = 0;

  glyph_info temp_info;

  for (char i = glyph_lower_; i <= glyph_upper_; i++) {
    glyph_index = FT_Get_Char_Index(face, i);
    e = FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);
    if (e) {
      // should have already been handled.
      temp_info.valid = false;
      glyph_cache_[i - glyph_lower_] = temp_info;
      continue;
    }

    // check bitmap format
    if (temp_glyph->format != FT_GLYPH_FORMAT_BITMAP) {
      FT_Render_Glyph(temp_glyph, FT_RENDER_MODE_NORMAL);
    }

    temp_info.valid = true;
    temp_glyph = face->glyph;
   
    // write to memory store
    int cursor_x;
    int cursor_y;
    if (temp_glyph->bitmap.pitch > 0) {
      cursor_x = width_px;
      cursor_y = 0;
      for (int i = 0; i < temp_glyph->bitmap.rows; i++, cursor_y++) {
        for (int j = 0; j < temp_glyph->bitmap.width; j++) {
          memory_cache_[cursor_y * atlas_width + cursor_x + j]
            = temp_glyph->bitmap.buffer[i * temp_glyph->bitmap.pitch + j];
        }
      }
    } else {
      cursor_x = width_px;
      cursor_y = height_px - temp_glyph->bitmap.rows;
      for (int i = 0; cursor_y < atlas_height; cursor_y++, i++) {
        for (int j = 0; j < temp_glyph->bitmap.width; j++) {
          memory_cache_[cursor_y * atlas_width + cursor_x + j]
            = temp_glyph->bitmap.buffer[j - (i * temp_glyph->bitmap.pitch)];
        }
      }
    }

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

  /**
   * TODO: can we be done with the face after ctor?
   * If so: we still want to keep the lib shared ptr
   * because it saves us lib constructions, the point is solely
   * to ensure that we have a lib to start, and destroy it before we're done.
   */
  {
    std::lock_guard<std::mutex> lock(ft_lib_lock_);
    FT_Done_Face(face);
  }
  
  // TODO: add ifdef for extension funcs
  // or if i'm feeling devilish
  // kick everyone out who doesn't support them
  // NOTE: dsa extension provides state during modification. nothing is bound to texture_2D afaik,
  // but we ARE telling the functions that this non-specific texture identifier is a texture 2d.
}

// advance is stored in 1/64 pixels
#define ADVANCE_SCALE 64.0f

model::Mesh<storage::VertexPacket2D> Font::GetTextGeometry(const std::string& text, float size_pt, TextFormat opts) const {
  // scales our fonts down to screenspace scale (roughly:)
  const float SCREENSPACE_FAC = (960.0f * bitmap_desired_scale) / size_pt;
  
  Mesh<VertexPacket2D> result;
  float origin_x = 0.0f;
  float origin_y = 0.0f;
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

  // for rearranging lines later
  float y_min = 0, y_max = 0;

  glyph_info* info;

  // for alignment: determines when the last line began.
  int last_line_start = 0;

  int cur = 0;
  for (auto c : text) {
    if (c == '\n') {
      origin_x = 0;
      // center all characters recorded thus far
      float x_width = 0;
      if (cur > 0) {
        x_width = result[(cur - 1) * 4 + 3].position.x;
      }

      switch (opts.horiz_align) {
        case CENTER:
          x_width /= 2;
        case RIGHT:
          break;
        case LEFT:
        default:
          x_width = 0;
          break;
      }

      for (int i = last_line_start * 4; i < cur * 4; i++) {
        result[i].position.x -= x_width;
      }

      last_line_start = cur;

      origin_y -= (line_height_ / (SCREENSPACE_FAC * ADVANCE_SCALE)); 
      continue;
    } else if (c < glyph_lower_ || c > glyph_upper_) {
      // skip, make space
      origin_x += (glyph_cache_[0].advance + opts.char_spacing) / (SCREENSPACE_FAC * ADVANCE_SCALE);
      continue;
    }

    info = &glyph_cache_[c - glyph_lower_];
    glyph_origin_x = origin_x + (info->bearing_x / SCREENSPACE_FAC);
    glyph_origin_y = origin_y + (info->bearing_y / SCREENSPACE_FAC);

    tex_width = ((float)info->width) / atlas_width;
    tex_height = ((float)info->height) / atlas_height;

    geom_width = info->width / SCREENSPACE_FAC;
    geom_height = info->height / SCREENSPACE_FAC;

    if (glyph_origin_y > y_max) {
      y_max = glyph_origin_y;
    }

    if (glyph_origin_y - geom_height < y_min) {
      y_min = glyph_origin_y;
    }

    result.AddVertex({glm::vec2(glyph_origin_x, glyph_origin_y),                            glm::vec2(info->origin_x, 0.0f)});
    result.AddVertex({glm::vec2(glyph_origin_x, glyph_origin_y - geom_height),              glm::vec2(info->origin_x, tex_height)});
    result.AddVertex({glm::vec2(glyph_origin_x + geom_width, glyph_origin_y - geom_height), glm::vec2(info->origin_x + tex_width, tex_height)});
    result.AddVertex({glm::vec2(glyph_origin_x + geom_width, glyph_origin_y),               glm::vec2(info->origin_x + tex_width, 0.0f)});
    result.AddPolygon(cur * 4, cur * 4 + 1, cur * 4 + 2);
    result.AddPolygon(cur * 4 + 2, cur * 4 + 3, cur * 4);

    cur++;

    origin_x += (info->advance / (SCREENSPACE_FAC * ADVANCE_SCALE));
  }

  if (cur != last_line_start) {
    float x_width = 0;
    if (cur > 0) {
      x_width = result[(cur - 1) * 4 + 3].position.x;
    }

    switch (opts.horiz_align) {
      case CENTER:
        x_width /= 2;
      case RIGHT:
        break;
      case LEFT:
      default:
        x_width = 0;
        break;
    }

    for (int i = last_line_start * 4; i < cur * 4; i++) {
      result[i].position.x -= x_width;
    }

    last_line_start = cur;
  }

  float v_alignment_offset;
  switch (opts.vert_align) {
    case TOP:
      v_alignment_offset = y_max;
      break;
    case MIDDLE:
      v_alignment_offset = (y_max + y_min) / 2;
      break;
    case BOTTOM:
      v_alignment_offset = y_min;
      break;
    case DEFAULT:
    default:
      v_alignment_offset = 0;
  }

  for (int i = 0; i < result.GetVertexCount(); i++) {
    result[i].position.y -= v_alignment_offset;
  }

  // account for vertical alignment

  return result;
}

GLuint Font::GetGlyphAtlas() const {
  // assume that this is being done so that we can performs some action on this value
  // and that someone isn't fucking around with me

  // upload the glyph atlas (we need it now, after all)
  if (memory_cache_ != nullptr) {
    // hasnt been uploaded yet :)
    // TBA: i could lock this, but all calls should be on the same thread, so no need.
    GLuint* glyph_texture = const_cast<GLuint*>(&glyph_texture_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, glyph_texture);
    glBindTexture(GL_TEXTURE_2D, *glyph_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_width, atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, memory_cache_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // unbind, not for any good reason but just because it makes me feel better
    glBindTexture(GL_TEXTURE_2D, 0);
    delete[] memory_cache_;
    char** memcache = const_cast<char**>(&memory_cache_);
    *memcache = nullptr;
  }
  return glyph_texture_;
}

Font::~Font() {
  if (glyph_cache_) {
    delete[] glyph_cache_;
  }

  if (memory_cache_) {
    delete[] memory_cache_;
  }

  if (glyph_texture_ != 0) {
    glDeleteTextures(1, &glyph_texture_);
  }
}

Font::Font(Font&& other) {
  ft_lib_ = other.ft_lib_;
  glyph_cache_ = other.glyph_cache_;
  other.glyph_cache_ = nullptr;
  memory_cache_ = other.memory_cache_;
  other.memory_cache_ = nullptr;
  glyph_texture_ = other.glyph_texture_;
  other.glyph_texture_ = 0;
  atlas_width = other.atlas_width;
  atlas_height = other.atlas_height;
}

Font& Font::operator=(Font&& other) {
  if (glyph_cache_ != nullptr) {
    delete[] glyph_cache_;
  }

  if (glyph_texture_ != 0) {
    glDeleteTextures(1, &glyph_texture_);
  }

  glyph_cache_ = other.glyph_cache_;
  other.glyph_cache_ = nullptr;
  memory_cache_ = other.memory_cache_;
  other.memory_cache_ = nullptr;
  glyph_texture_ = other.glyph_texture_;
  other.glyph_texture_ = 0;
  atlas_width = other.atlas_width;
  atlas_height = other.atlas_height;
  return *this;
}

}
}