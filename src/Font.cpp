#include "Font.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <boost/log/trivial.hpp>
#include <iostream>

namespace screenspacemanager {
int Font::InitializeFont(Font** font, std::string font_name) {
  return InitializeFont(font, font_name, 128);
}

int Font::InitializeFont(Font** font, std::string font_name, int char_size) {
  FT_Library lib;

  FT_Int err = FT_Init_FreeType(&lib);
  if (err) {
    // non-ideal :/
    // move this over to another logging solution later
    BOOST_LOG_TRIVIAL(error) << "Error encountered while creating FT_Lib!\n" <<
      FT_Error_String(err);
    return -1;
  }

  FT_Face face;
  err = FT_New_Face(lib, font_name.c_str(), 0, &face);

  if (err) {
    BOOST_LOG_TRIVIAL(error) << "Error encountered while instantiating new face for " << face << "\n" <<
      FT_Error_String(err);
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
  FT_Int err;

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
        err = FT_Load_Char(face, index, FT_LOAD_RENDER);
        if (err) {
          BOOST_LOG_TRIVIAL(warning) << "Skipping character " << (char)index;
          continue;
        }

        // leave space, otherwise filtering generates weird edges.
        width += glyph->bitmap.width + 1;
        height = (glyph->bitmap.rows > height ? glyph->bitmap.rows : height);
        // debug
        BOOST_LOG_TRIVIAL(debug) << (char)index << ": " << glyph->bitmap.width << " x " << glyph->bitmap.rows;
        break;
    }
  }

  uint32_t write_x = 0;

  tex_width_ = width;
  tex_height_ = height;

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
        if (err) {
          BOOST_LOG_TRIVIAL(warning) << "Skipping character " << (char)index;
          continue;
        }

        // this should be fine
        glTexSubImage2D(GL_TEXTURE_2D, 0, width, 0, glyph->bitmap.width, glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);
        glyphs_[index - INDEX_START] = {
          static_cast<float>(write_x) / width,
          glyph->bitmap.width / width,
          glyph->bitmap.rows / height,
          glyph->bitmap_left,
          glyph->bitmap_top,
          glyph->advance.x << 6   // scale from freepoint scale to pixel scale
        };

        width += (glyph->bitmap.width) + 1;
        
        break;   
    }
  }
}

std::vector<float> Font::GenerateAttribArray(const std::string& input, float scale, float origin_x, float origin_y, std::vector<uint32_t>* indices) {
  std::vector<float> data;
  int counter = 0;
  for (char c : input) {
    // pass to charattribarray
    // it does all the work lol
    origin_x = GenerateCharAttribArray(c, data, origin_x, origin_y, scale);

    // push indices list
    indices->push_back(4 * counter);
    indices->push_back(4 * counter + 1);
    indices->push_back(4 * counter + 2);

    indices->push_back(4 * counter + 1);
    indices->push_back(4 * counter + 3);
    indices->push_back(4 * counter + 2);

    counter++;
  } 

  // how to return indices list? or should this method save it for later?
  return data;
}



float Font::GenerateCharAttribArray(char c, std::vector<float>& data, float origin_x, float origin_y, float scale) {
  if (c < INDEX_START || c >= INDEX_END) {
    // invalid character.
    return origin_x;
  } else {
    FontGlyph glyph = glyphs_[c - INDEX_START];

    float x = origin_x + (glyph.bearing_x * scale);                 // left x
    float y = origin_y - (glyph.bearing_y * scale);                 // top y
    float dx = (glyph.width * scale);                               // char width on screen
    float dy = (glyph.height * scale);                              // char height on screen

    float tx = glyph.start;                                         // left tx
    float ty = 0;                                                   // top ty
    float tdx = (glyph.width / static_cast<float>(tex_width_));     // tex width in tcoord
    float tdy = (glyph.height / static_cast<float>(tex_height_));   // tex height in tcoord

    // top left
    data.push_back(x);
    data.push_back(y);
    data.push_back(tx);
    data.push_back(ty);

    // bottom left
    data.push_back(x);
    data.push_back(y + dy);
    data.push_back(tx);
    data.push_back(ty + tdy);

    // top right
    data.push_back(x + dx);
    data.push_back(y);
    data.push_back(tx + tdx);
    data.push_back(ty);

    // bottom right
    data.push_back(x + dx);
    data.push_back(y + dy);
    data.push_back(tx + tdx);
    data.push_back(ty + tdy);

    return origin_x + (glyph.advance * scale);

    // 0, 1, 2
    // 1, 3, 2

  }
};

};  // namespace screenspacemanager
