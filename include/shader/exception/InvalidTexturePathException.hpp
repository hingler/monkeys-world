#ifndef INVALID_TEXTURE_PATH_EXCEPTION_H_
#define INVALID_TEXTURE_PATH_EXCEPTION_H_

#include <string>
#include <stdexcept>

namespace monkeysworld {
namespace shader {
namespace exception {

class InvalidTexturePathException : public std::runtime_error {
 public:
  InvalidTexturePathException(const std::string& arg) : runtime_error(arg) {}
  InvalidTexturePathException(const char* arg) : runtime_error(arg) {}
};

}
}
}