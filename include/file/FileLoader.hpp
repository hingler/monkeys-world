#ifndef FILE_LOADER_H_
#define FILE_LOADER_H_
#include <memory>
#include <string>

namespace monkeysworld {
namespace file {

/**
 *  Interface for file loader
 */ 
class FileLoader {
 public:
 /**
   *  Returns a stream buffer corresponding with the desired file.
   *  Updates internal cache if necessary.
   *  @param path - the path to the desired file.
   *  If the file has not been loaded, spins until the file has been cached, then returns.
   */ 
  virtual std::unique_ptr<std::streambuf> LoadFile(const std::string& path) = 0;
};

} // namespace file
} // namespace monkeysworld

#endif