#ifndef UNINITIALIZED_SHADER_EXCEPTION_H_
#define UNINITIALIZED_SHADER_EXCEPTION_H_

#include <string>
#include <stdexcept>

namespace monkeysworld {
namespace shader {
namespace exception {

/**
 *  Used when a program descriptor is fetched for a shader whose program ID is still invalid
 */ 
class UninitializedShaderException : public std::runtime_error {
 public:
  UninitializedShaderException(const std::string& arg) : runtime_error(arg) {}
  UninitializedShaderException(const char* arg) : runtime_error(arg) {}

};

}   // namespace exception
}   // namespace shader
}   // namespace monkeysworld

#endif  // UNINITIALIZED_SHADER_EXCEPTION_H_