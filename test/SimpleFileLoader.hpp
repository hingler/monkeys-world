#ifndef SIMPLE_FILE_LOADER_H_
#define SIMPLE_FILE_LOADER_H_

#include <file/CachedFileLoader.hpp>

#include <fstream>
#include <memory>
#include <string>

// simple interface for file loader
class SimpleFileLoader : public ::monkeysworld::file::CachedFileLoader {
  virtual std::unique_ptr<std::streambuf> LoadFile(const std::string& path) override {
    std::filebuf* res = new std::filebuf;
    res->open(path.c_str(), std::ios_base::in);
    return std::unique_ptr<std::streambuf>(res);
  }
};

#endif // SIMPLE_FILE_LOADER_H_