#include <file/CacheStreambuf.hpp>

#include <iostream>

namespace screenspacemanager {
namespace file {

using std::ios_base;

CacheStreambuf::CacheStreambuf(const std::shared_ptr<std::vector<char>>& data) : data_(data), getc_(0) {}


std::streampos CacheStreambuf::seekoff(std::streamoff off, ios_base::seekdir way, ios_base::openmode which) {
  // shouldn't do anything
  if (which == ios_base::out) {
    return -1;
  }

  char* data_head = const_cast<char*>(data_->data());

  switch (way) {
    case ios_base::beg:
      if (off >= data_->size()) {
        // failure case?
        std::streampos(std::streamoff(-1));
      }
      getc_ = off;
      break;
    case ios_base::cur:
      if (getc_ + off >= data_->size()) {
        std::streampos(std::streamoff(-1));
      }
      getc_ += off;
      break;
    case ios_base::end:
      if (data_->size() - off < 0) {
        std::streampos(std::streamoff(-1));
      }

      getc_ = data_->size() - off;
  }

  setg(data_head, data_head + getc_, data_head + data_->size());
  return std::streampos(getc_);
}

std::streampos CacheStreambuf::seekpos(std::streampos sp, ios_base::openmode which) {
  if (which == ios_base::out) {
    return -1;
  }
  return seekoff(std::streamoff(sp), ios_base::beg, which);
}

std::streamsize CacheStreambuf::showmanyc() {
  return data_->size() - getc_;
}

std::streamsize CacheStreambuf::xsgetn(char* s, std::streamsize n) {
  std::streamsize max_read = std::min(n, showmanyc());
  if (max_read > 0) {
    
    for (int i = 0; i < max_read; i++) {
      s[i] = data_->operator[](getc_ + i);
    }

    getc_ += max_read;
  }

  return max_read;
}

CacheStreambuf::int_type CacheStreambuf::underflow() {
  if (getc_ >= data_->size()) {
    return traits_type::eof();
  }
  
  char* last_accessed = const_cast<char*>(&data_->operator[](getc_));
  setg(last_accessed, last_accessed, last_accessed + (data_->size() - getc_));
  
  getc_ = data_->size();
  return traits_type::to_int_type(*last_accessed);
}

CacheStreambuf::int_type CacheStreambuf::uflow() {
  if (getc_ >= data_->size()) {
    return traits_type::eof();
  }

  getc_++;
  if (underflow() == traits_type::eof()) {
    return traits_type::eof();
  }

  // underflow, in success case, will set gptr to char ahead of desired
  return gptr()[-1];
}

CacheStreambuf::int_type CacheStreambuf::pbackfail(int_type c) {
  if (getc_ <= 0) {
    return traits_type::eof();
  }

  return traits_type::to_int_type(data_->operator[](--getc_));
}

std::streamsize CacheStreambuf::xsputn(const char* s, std::streamsize n) {
  return 0;
}

CacheStreambuf::int_type CacheStreambuf::overflow(CacheStreambuf::int_type c) {
  return traits_type::eof();
}

} // namespace file
} // namespace screenspacemanager