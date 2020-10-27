#include <file/FileLoader.hpp>

#include <fstream>
#include <memory>
#include <string>

// simple interface for file loader
class SimpleFileLoader : ::screenspacemanager::file::FileLoader {
  virtual std::unique_ptr<std::streambuf> LoadFile(const std::string& path) override {
    std::filebuf* res = new std::filebuf;
    res->open(path.c_str());
    return std::unique_ptr<std::streambuf>(res);
  }
}