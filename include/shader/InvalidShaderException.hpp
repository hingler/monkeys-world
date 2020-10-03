#ifndef INVALID_SHADER_EXCEPTION_H_
#define INVALID_SHADER_EXCEPTION_H_

#include <string>

namespace screenspacemanager {
namespace shader {

// separate "exception" namespace?
class InvalidShaderException : public std::runtime_error {
 public:
  InvalidShaderException(const std::string& arg) : runtime_error(arg) {}
  InvalidShaderException(const char* arg) : runtime_error(arg) {}

};

}   // namespace shader
}   // namespace screenspacemanager

#endif  // INVALID_SHADER_EXCEPTION_H_