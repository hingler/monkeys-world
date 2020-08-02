// fix this later lol
#include "Font.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

int Font::InitializeFont(Font** font, std::string font_name) {
  return InitializeFont(font, font_name, 128);
}

int Font::InitializeFont(Font** font, std::string font_name, int char_size) {
  FT_Library lib;

  FT_Int error = FT_Init_FreeType(&lib);
  if (error) {
    // non-ideal :/
    // move this over to another logging solution later
    std::cout << "Error encountered while creating FT_Lib!" << std::endl;
    std::cout << FT_Error_String(error) << std::endl;
    return -1;
  }

  FT_Face face;
  error = FT_New_Face(lib, font_name.c_str(), 0, &face);

  if (error) {
    std::cout << "Error encountered while instantiating new face for " << face << std::endl;
    std::cout << FT_Error_String(error) << std::endl;
    return -1;
  }

  // sizes are figured

  // just do everything here
  // if an error occurs, -1 and don't generate the font object
  // if not, keep going!

  // load char into face
  // get glyph slot from face
  // that underlying bitmap is what we want to use

  *font = new Font(font_name, char_size, face);
  return 0;
}

Font::Font(std::string font, int char_size, FT_Face face) {
  FT_Int error;

  uint32_t width = 0;
  uint32_t height = 0;

  glyphs_ = std::make_unique<FontGlyph[]>(INDEX_END - INDEX_START);

  FT_GlyphSlot glyph = face->glyph;

  FT_Set_Pixel_Sizes(face, char_size, 0);

  for (int index = INDEX_START; index < INDEX_END; index++) {
    switch(index) {
      case 127:
        continue;
        break;
      default:
        error = FT_Load_Char(face, index, FT_LOAD_RENDER);
        if (error) {
          std::cout << "Skipping character " << index << std::endl;
          continue;
        }

        // leave space, otherwise filtering generates weird edges.
        width += glyph->bitmap.width + 1;
        height = (glyph->bitmap.rows > height ? glyph->bitmap.rows : height);
        // debug
        std::cout << (char)index << ": " << glyph->bitmap.width << " x " << glyph->bitmap.rows << std::endl;
        break;
    }
  }

  width = 0;

  glGenTextures(1, &tex_);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  for (int index = INDEX_START; index < INDEX_END; index++) {
    switch(index) {
      case 127:
        continue;
        break;
      default:
        FT_Load_Char(face, index, FT_LOAD_RENDER);
        if (error) {
          std::cout << "Skipping character " << index << std::endl;
          continue;
        }

        // this should be fine
        glTexSubImage2D(GL_TEXTURE_2D, 0, width, 0, glyph->bitmap.width, glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);
        glyphs_[index - INDEX_START] = {
          glyph->bitmap.width,
          glyph->bitmap.rows,
          glyph->bitmap_left,   // these are usable for bearing for now
          glyph->bitmap_top,
          glyph->advance.x
        };

        width += (glyph->bitmap.width) + 1;
        
        break;   
    }
  }

  tex_width_ = width;
}