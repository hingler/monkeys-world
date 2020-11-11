#include <utils/FileUtils.hpp>

#include <atomic>
#include <cinttypes>
#include <mutex>

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace utils {
namespace fileutils {

static const uint32_t CRC_MAGIC = 0x04C11DB7;

uint32_t CalculateCRCHash(std::istream& input, std::streamoff offset) {
  // todo: get this to store, lock/unlock properly
  uint32_t crc_table[256];
  uint32_t crc;

  for (int i = 0; i < 256; i++) {
    crc = i;
    for (int j = 0; j < 8; j++) {
      if (crc & 1) {
        crc >>= 1;
        crc ^= CRC_MAGIC;
      } else {
        crc >>= 1;
      }
    }
    crc_table[i] = crc;
  }

  crc = 0xFFFFFFFF;
  char c;
  std::streampos pos_initial = input.tellg();
  input.seekg(offset);

  while ((c = input.get()) != EOF) {
    crc = (crc >> 8) ^ crc_table[(c ^ crc) & 0xFF];
  }

  input.clear();
  input.seekg(pos_initial);
  return ~crc;
}

} // namespace fileutils
} // namespace utils
} // namespace monkeysworld