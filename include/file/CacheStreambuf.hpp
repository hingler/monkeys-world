#ifndef CACHE_STREAMBUF_H_
#define CACHE_STREAMBUF_H_

#include <memory>
#include <streambuf>
#include <vector>

namespace screenspacemanager {
namespace file {

class CacheStreambuf : public std::streambuf {

 public:
  CacheStreambuf(const std::shared_ptr<const std::vector<char>>& data);

  // returns number of characters available on internal input buf
  std::streamsize showmanyc() override;

  // writes n chars to s from internal input buf
  std::streamsize xsgetn(char* s, std::streamsize n) override;

  // returns next char and 
  CacheStreambuf::int_type underflow() override;
  CacheStreambuf::int_type uflow() override;
  CacheStreambuf::int_type pbackfail(CacheStreambuf::int_type c) override;

  // zeroes out inherited output methods to ensure that writing does not occur
  std::streamsize xsputn(const char* s, std::streamsize n) override;
  int_type overflow(int_type c) override;
 private:
  const std::shared_ptr<const std::vector<char>> data_;
  std::streamsize getc_;   
};

} // namespace file
} // namespace screenspacemanager

#endif  // CACHE_STREAMBUF_H_