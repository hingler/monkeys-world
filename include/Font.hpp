#include <string>

#include <glad/glad.h>
#include <ft2build.h>
#include <cinttypes>

#include <memory>
#include <vector>

#include FT_FREETYPE_H

struct FontGlyph {
  float start;        // x position of the char's origin relative to the texture [0 - 1)

  uint32_t width;     // bitmap width in px
  uint32_t height;    // bitmap height in px

  FT_Int bearing_x;   // horizontal displacement from origin in px
  FT_Int bearing_y;   // vertical displacement from baseline in px

  FT_Pos advance;     // horizontal origin delta
};

/**
 *  Stores all resources required for loading and drawing fonts on-screen.
 */ 
class Font {
 public:

  /**
   *  Overload for InitializeFont with font size set to 128 px wide.
   */ 
  static int InitializeFont(Font** font, std::string font_name);

  /**
   *  Creates a new Font instance.
   *  @param font - output parameter; populated with the address of the newly created font.
   *  @param font_name - 
   */ 
  static int InitializeFont(Font** font, std::string font_name, int font_size);

  /**
   *  Exposes the internal texture atlas.
   *  @return The GLuint corresponding with the texture atlas.
   */ 
  GLuint GetTexture();

  /**
   *  Generates the vertex attributes necessary to render the text element.
   *  Returned format:
   *    - 8 bytes -- XY position for vertex.
   *    - 8 bytes -- UV texCoords for vertex.
   */ 
  std::vector<float> GenerateAttribArray(std::string input, float scale, float origin_x, float origin_y);
 private:
  /**
   *  Create a font object from an inputted font and char size
   */ 
  Font(std::string font, int char_size, FT_Face face);

  /**
   *  Appends a new character to the vertex attrib array.
   *  Returns the new x origin
   * 
   *  @param c - the character we are generating
   *  @param data - output parameter for vertex data
   *  @param origin_x - x coord of origin
   *  @param origin_y - y coord of origin
   *  @param scale - value to multiply text size by (relative to text size. make absolute later :))
   */ 
  float GenerateCharAttribArray(char c, std::vector<float>& data, float origin_x, float origin_y, float scale);

  // specialized array template
  std::unique_ptr<FontGlyph[]> glyphs_;
  GLuint tex_;

  uint32_t tex_width_;
  uint32_t tex_height_;

  const static int INDEX_START = 32;
  const static int INDEX_END = 256;
};