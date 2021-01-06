#ifndef BAD_FONT_PATH_EXCEPTION_H_
#define BAD_FONT_PATH_EXCEPTION_H_

#include <string>
#include <stdexcept>

namespace monkeysworld {
namespace font {
namespace exception {

class BadFontPathException : public std::runtime_error {
 public:
  BadFontPathException(const std::string& arg) : runtime_error(arg) {}
  BadFontPathException(const char* arg) : runtime_error(arg) {}
};

}
}
}

#endif