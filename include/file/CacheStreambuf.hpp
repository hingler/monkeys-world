#ifndef CACHE_STREAMBUF_H_
#define CACHE_STREAMBUF_H_

#include <memory>
#include <streambuf>
#include <vector>

namespace monkeysworld {
namespace file {

class CacheStreambuf : public std::streambuf {

 public:
  CacheStreambuf();
  CacheStreambuf(const std::shared_ptr<std::vector<char>>& data);

  // -1 on failure, abs pos on success
  std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) override;
  std::streampos seekpos(std::streampos sp, std::ios_base::openmode which) override;

  std::streamsize showmanyc() override;

  // zeroes out inherited output methods to ensure that writing does not occur
  std::streamsize xsputn(const char* s, std::streamsize n) override;
  int_type overflow(int_type c) override;
  int_type underflow() override;

  CacheStreambuf(const CacheStreambuf& other);
  CacheStreambuf& operator=(const CacheStreambuf& other);
  CacheStreambuf(CacheStreambuf&& other);
  CacheStreambuf& operator=(CacheStreambuf&& other);
 private:
  const std::shared_ptr<std::vector<char>> data_;
};

} // namespace file
} // namespace monkeysworld

#endif  // CACHE_STREAMBUF_H_