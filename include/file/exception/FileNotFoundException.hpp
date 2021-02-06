#ifndef FILE_NOT_FOUND_EXCEPTION_H_
#define FILE_NOT_FOUND_EXCEPTION_H_

#include <string>
#include <stdexcept>

namespace monkeysworld {
namespace file {
namespace exception {

class FileNotFoundException : public std::runtime_error {
 public:
  FileNotFoundException(const std::string& arg) : runtime_error(arg) {}
  FileNotFoundException(const char* arg) : runtime_error(arg) {}
};

}
}
}

#endif