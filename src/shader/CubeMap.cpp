#include <shader/CubeMap.hpp>
#include <shader/exception/InvalidTexturePathException.hpp>

#include <boost/log/trivial.hpp>

#include <GLFW/glfw3.h>

namespace monkeysworld {
namespace shader {

CubeMap::CubeMap(std::string x_pos,
                 std::string x_neg,
                 std::string y_pos,
                 std::string y_neg,
                 std::string z_pos,
                 std::string z_neg) {
  face_info* data = face_data_;
  std::string* paths[6] = {&x_pos, &x_neg, &y_pos, &y_neg, &z_pos, &z_neg};
  cubemap_ = 0;

  for (int i = 0; i < 6; i++) {
    data[i].data = stbi_load(paths[i]->c_str(), &data[i].width, &data[i].height, &data[i].channels, 0);
    if (!data[i].data) {
      BOOST_LOG_TRIVIAL(error) << "invalid skybox texture: " << i;
      throw exception::InvalidTexturePathException("invalid skybox texture path");
    }
  }
}

uint64_t CubeMap::GetCubemapSize() const {
  const face_info* data = face_data_;
  uint64_t res = 0;
  for (int i = 0; i < 6; i++, data++) {
    res += (data->channels * data->height * data->width);
  }

  return res;
}

GLuint CubeMap::GetCubeMapDescriptor() const {
  if (cubemap_ == 0) {
    GLuint* cubemap_ref = const_cast<GLuint*>(&cubemap_);
    glGenTextures(1, cubemap_ref);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_);
    const static GLenum targets[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                                      GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                      GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                                      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                      GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                                      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
    face_info* data = const_cast<face_info*>(face_data_);
    GLenum format;
    for (int i = 0; i < 6; i++, data++) {
      switch (data->channels) {
        case 1:
          format = GL_RED;
          break;
        case 3:
          format = GL_RGB;
          break;
        case 4:
          format = GL_RGBA;
          break;
        default:
          // not sure
          format = GL_RED;
          break;
      }

      glTexImage2D(targets[i], 0, format, data->width, data->height, 0, format, GL_UNSIGNED_BYTE, data->data);
      stbi_image_free(data->data);
      data->data = nullptr;
    }
  }

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return cubemap_;
}

CubeMap::~CubeMap() {
  if (cubemap_ != 0) {
    if (glfwGetCurrentContext()) {
      glDeleteTextures(1, &cubemap_);
    } else {
      BOOST_LOG_TRIVIAL(warning) << "cubemap descriptor could not be destroyed!";
    }
  }

  for (int i = 0; i < 6; i++) {
    if (face_data_[i].data != nullptr) {
      stbi_image_free(face_data_[i].data);
    }
  }
}

}
}