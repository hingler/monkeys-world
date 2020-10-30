#include <file/CacheStreambuf.hpp>

#include <iostream>

namespace monkeysworld {
namespace file {

// contract: always expose the full cached vector

using std::ios_base;

CacheStreambuf::CacheStreambuf(const std::shared_ptr<std::vector<char>>& data) : data_(data) {
  char* data_ptr = data_->data();
  setg(data_ptr, data_ptr, data_ptr + data_->size());
}


std::streampos CacheStreambuf::seekoff(std::streamoff off, ios_base::seekdir way, ios_base::openmode which) {
  if (which == ios_base::out) {
    return -1;
  }

  char* data_head = const_cast<char*>(data_->data());
  std::streampos offset;
  switch (way) {
    case ios_base::beg:
      offset = off;
      break;
    case ios_base::cur:
      offset = (gptr() - data_->data()) + off;
      break;
    case ios_base::end:
      offset = data_->size() - off;
  }

  setg(data_head, data_head + offset, data_head + data_->size());
  return offset;
}

std::streampos CacheStreambuf::seekpos(std::streampos sp, ios_base::openmode which) {
  if (which == ios_base::out) {
    return -1;
  }
  return seekoff(std::streamoff(sp), ios_base::beg, which);
}

std::streamsize CacheStreambuf::showmanyc() {
  return 0;
}

std::streamsize CacheStreambuf::xsputn(const char* s, std::streamsize n) {
  return 0;
}

CacheStreambuf::int_type CacheStreambuf::overflow(CacheStreambuf::int_type c) {
  return traits_type::eof();
}

} // namespace file
} // namespace monkeysworld