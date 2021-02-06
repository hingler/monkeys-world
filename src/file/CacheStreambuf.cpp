#include <file/CacheStreambuf.hpp>
#include <file/exception/FileNotFoundException.hpp>

#include <iostream>

namespace monkeysworld {
namespace file {

// contract: always expose the full cached vector

using std::ios_base;
using exception::FileNotFoundException;

CacheStreambuf::CacheStreambuf() : data_() {
  setg(nullptr, nullptr, nullptr);
}

CacheStreambuf::CacheStreambuf(const std::shared_ptr<std::vector<char>>& data) : data_(data) {
  char* data_ptr = data_->data();
  setg(data_ptr, data_ptr, data_ptr + data_->size());
}

std::streampos CacheStreambuf::seekoff(std::streamoff off, ios_base::seekdir way, ios_base::openmode which) {
  if (!data_) {
    throw FileNotFoundException("streambuf not valid");
  }

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
  if (!data_) {
    throw FileNotFoundException("streambuf not valid");
  }
  
  if (which == ios_base::out) {
    return -1;
  }
  return seekoff(std::streamoff(sp), ios_base::beg, which);
}

std::streamsize CacheStreambuf::showmanyc() {
  if (!data_) {
    throw FileNotFoundException("streambuf not valid");
  }

  return 0;
}

std::streamsize CacheStreambuf::xsputn(const char* s, std::streamsize n) {
  if (!data_) {
    throw FileNotFoundException("streambuf not valid");
  }

  return 0;
}

CacheStreambuf::int_type CacheStreambuf::overflow(CacheStreambuf::int_type c) {
  if (!data_) {
    throw FileNotFoundException("streambuf not valid");
  }

  return traits_type::eof();
}

CacheStreambuf::int_type CacheStreambuf::underflow() {
  std::cout << "underflow" << std::endl;
  if (!data_) {
    std::cout << "hello!" << std::endl;
    throw FileNotFoundException("streambuf not valid");
  }

  char* ptr = gptr();
  if (ptr == (data_->data() + data_->size())) {
    return EOF;
  }

  return *ptr;
  
}

CacheStreambuf::CacheStreambuf(const CacheStreambuf& other) : std::streambuf(other),
                                                              data_(other.data_) {
  setg(other.eback(), other.gptr(), other.egptr());
}

CacheStreambuf& CacheStreambuf::operator=(const CacheStreambuf& other) {
  std::shared_ptr<std::vector<char>>& data = const_cast<std::shared_ptr<std::vector<char>>&>(data_);
  data = other.data_;
  setg(other.eback(), other.gptr(), other.egptr());
  return *this;
}

CacheStreambuf::CacheStreambuf(CacheStreambuf&& other) : data_(std::move(other.data_)) {
  setg(other.eback(), other.gptr(), other.egptr());
}

CacheStreambuf& CacheStreambuf::operator=(CacheStreambuf&& other) {
  std::shared_ptr<std::vector<char>>& data = const_cast<std::shared_ptr<std::vector<char>>&>(data_);
  data = std::move(other.data_);
  setg(other.eback(), other.gptr(), other.egptr());
  return *this;
}

} // namespace file
} // namespace monkeysworld