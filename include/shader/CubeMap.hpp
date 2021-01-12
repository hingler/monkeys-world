// note: cubemaps require 1-6 files -- we need a way to store this info in our preexisting cached file structure
// i'd just say: swizzle the name or something
// we could separate names with colons, i cant think of a case where i NEED a colon in a skybox texture name
// store a cache record containing all six files meshed together, and then recall all at once
// maybe add a leading colon so we can quickly deduce that the file name is a sequence of six

#ifndef CUBE_MAP_H_
#define CUBE_MAP_H_

#include <stb_image.h>

#include <glad/glad.h>

#include <string>

namespace monkeysworld {
namespace shader {

/**
 *  Represents a cube map.
 */ 
class CubeMap {

 public:
  /**
   *  Creates a new cubemap from a set of six faces.
   *  @param x_pos - +X face
   *  @param x_neg - -X face
   *  @param y_pos - +Y face
   *  @param y_neg - -Y face
   *  @param z_pos - +Z face
   *  @param z_neg - -Z face
   */ 
  CubeMap(std::string x_pos,
          std::string x_neg,
          std::string y_pos,
          std::string y_neg,
          std::string z_pos,
          std::string z_neg);

  /**
   *  @returns the descriptor associated with this cubemap texture.
   */ 
  GLuint GetCubeMapDescriptor() const;

  /**
   *  @returns the size, in bytes, of the cubemap
   */ 
  uint64_t GetCubemapSize() const;

  /**
   *  CubeMap dtor
   */ 
  ~CubeMap();
 private:

  struct face_info {
    stbi_uc* data;
    int width;
    int height;
    int channels;
  };

  GLuint cubemap_;
  // memory cache for input data
  face_info x_pos_data_;
  face_info x_neg_data_;
  face_info y_pos_data_;
  face_info y_neg_data_;
  face_info z_pos_data_;
  face_info z_neg_data_;

  // a list of ordered targets for the cubemap to make uploading textures a bit neater
  const static GLenum targets_[6];

};

}
}

#endif