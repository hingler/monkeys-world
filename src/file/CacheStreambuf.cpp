#include <file/CacheStreambuf.hpp>

namespace screenspacemanager {
namespace file {

// TODO: swap out getc with raw data ptrs and keep shared ptr to data for ownership
CacheStreambuf::CacheStreambuf(const std::shared_ptr<const std::vector<char>>& data) : data_(data), getc_(0) {
  const char* cache_data = data->data();
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
    return traits_type::to_int_type(EOF);
  }
  
  char* last_accessed = const_cast<char*>(&data_->operator[](getc_));
  setg(last_accessed, last_accessed, last_accessed + (data_->size() - getc_));
  getc_ = data_->size();
  return traits_type::to_int_type(*last_accessed);
}

CacheStreambuf::int_type CacheStreambuf::uflow() {
  getc_++;
  if (underflow() == traits_type::to_int_type(EOF)) {
    return traits_type::to_int_type(EOF);
  }

  // underflow, in success case, will set gptr to char ahead of desired
  return gptr()[-1];
}

CacheStreambuf::int_type CacheStreambuf::pbackfail(int_type c) {
  if (getc_ <= 0) {
    return traits_type::to_int_type(EOF);
  }

  return traits_type::to_int_type(data_->operator[](--getc_));
}

std::streamsize CacheStreambuf::xsputn(const char* s, std::streamsize n) {
  return 0;
}

CacheStreambuf::int_type CacheStreambuf::overflow(CacheStreambuf::int_type c) {
  return CacheStreambuf::traits_type::to_int_type(EOF);
}

} // namespace file
} // namespace screenspacemanager