#ifndef FONT_H_
#define FONT_H_

#include <ft2build.h>
#include FT_FREETYPE_H

#include <font/FTLibWrapper.hpp>

#include <glad/glad.h>

#include <model/Mesh.hpp>
#include <storage/VertexPacketTypes.hpp>

#include <mutex>
#include <string>

namespace monkeysworld {
namespace font {

struct glyph_info {
  // dimensions of glyphs in pixels
  int width;
  int height;

  // x/y offset from text origin
  int bearing_x;
  int bearing_y;

  // dist to advance origin by for next char (horiz only for now)
  float advance;

  // start of glyph on x axis in texture coords
  float origin_x;

  // false for characters which did not load correctly
  bool valid;
};

/**
 *  Represents a font and all of its glyphs, returning information pertaining to textures, etc.
 */ 
class Font {
 public:
  /**
   *  Creates a new Font object.
   *  @param font_name - the path to the desired font.
   */ 
  Font(const std::string& font_path);

  /**
   *  Generates and returns geometry from text. Initial origin is always <0, 0, 0>, and the glyphs are projected onto the XY plane.
   *  @param text - the message being read.
   *  @param size_pt - the size of the text, in pt.
   *  @returns A 3D mesh corresponding with the desired text. Texture coordinates correspond with the
   *           glyph atlas (see GetGlyphAtlas()).
   */ 
  std::shared_ptr<model::Mesh<storage::VertexPacket2D>> GetTextGeometry(const std::string& text, float size_pt) const;
  // probably add a param for some simple font formatting :)

  /**
   *  Gets the glyph atlas associated with this font.
   *  @returns a GL descriptor associated with the underlying font atlas.
   */ 
  GLuint GetGlyphAtlas() const;

  ~Font();
  Font(const Font& other) = delete;
  Font& operator=(const Font& other) = delete;
  Font(Font&& other);
  Font& operator=(Font&& other);
 private:
  // bounds for glyphs
  const char glyph_lower_ = 0x20;
  const char glyph_upper_ = 0x7e;
  // size of loader glyphs
  const int bitmap_desired_scale = 256;

  /**
   *  MANAGING A LIBRARY
   *  
   *  From here, we have no concept of consistent threads, forcing us to use locks
   *  to access a shared FT_Library across all font instances.
   * 
   *  However, this is difficult to actually do.
   * 
   *  What we want is an RAII singleton class which wraps `FT_Library`,
   *  and (i guess?) a static weak_ptr which stores it (wrapping init_freetype and any cleanup funcs)
   * 
   *  - Why a weak ptr?
   *    - seems like a better way to ensure that we initialize it when we get there, and de-initialize when we leave.
   * 
   *  First, we grab a mutex lock, and attempt to lock the ptr.
   *  If that fails, then we create a new instance of `Library,` and continue using the lock.
   *  If it succeeds, then we store the resulting shared ptr in the class.
   */ 
  // weak ptr to static shared lib
  static std::weak_ptr<FTLibWrapper> lib_singleton_;
  // local shared ptr to lib
  std::shared_ptr<FTLibWrapper> ft_lib_;
  // static mutex for shared commands
  static std::mutex ft_lib_lock_;
  glyph_info* glyph_cache_;
  // TBA: stores glyphs in-memory so they can be xfer'd to GPU all at once
  char* memory_cache_;
  GLuint glyph_texture_;

  int atlas_width;
  int atlas_height;
};

}
}

#endif