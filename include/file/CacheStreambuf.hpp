#ifndef CACHE_STREAMBUF_H_
#define CACHE_STREAMBUF_H_

#include <memory>
#include <streambuf>
#include <vector>

namespace screenspacemanager {
namespace file {

class CacheStreambuf : public std::streambuf {

 public:
  CacheStreambuf(const std::shared_ptr<std::vector<char>>& data);

  // -1 on failure, abs pos on success
  std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) override;
  std::streampos seekpos(std::streampos sp, std::ios_base::openmode which) override;

  std::streamsize showmanyc() override;
  std::streamsize xsgetn(char* s, std::streamsize n) override;
  CacheStreambuf::int_type underflow() override;
  CacheStreambuf::int_type uflow() override;
  CacheStreambuf::int_type pbackfail(CacheStreambuf::int_type c) override;

  // zeroes out inherited output methods to ensure that writing does not occur
  std::streamsize xsputn(const char* s, std::streamsize n) override;
  int_type overflow(int_type c) override;
 private:
  const std::shared_ptr<std::vector<char>> data_;
  std::streamoff getc_;   
};

} // namespace file
} // namespace screenspacemanager

#endif  // CACHE_STREAMBUF_H_