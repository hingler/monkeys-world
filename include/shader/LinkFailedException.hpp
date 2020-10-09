#ifndef LINK_FAILED_EXCEPTION_H_
#define LINK_FAILED_EXCEPTION_H_

#include <string>
#include <exception>

namespace screenspacemanager {
namespace shader {

/**
 *  Used when linking a program fails
 */ 
class LinkFailedException : public std::runtime_error {
 public:
  LinkFailedException(const std::string& arg) : runtime_error(arg) {}
  LinkFailedException(const char* arg) : runtime_error(arg) {}

};

}   // namespace shader
}   // namespace screenspacemanager

#endif  // LINK_FAILED_EXCEPTION_H_