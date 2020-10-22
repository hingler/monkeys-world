#include <utils/FileUtils.hpp>

#include <cinttypes>

namespace screenspacemanager {
namespace utils {
namespace fileutils {

// stores a record of CRC hashes.
// a lock on writing this static data should be unnecessary
// but we can deal with it later :)
static uint32_t CRC_HASH[256];

static bool CRC_HASH_CACHED = false;

static const uint32_t CRC_MAGIC = 0x04C11DB7;

uint32_t CalculateCRCHash(std::istream& input, std::streamoff offset) {
  if (!CRC_HASH_CACHED) {
    // generate the hash table

    uint32_t crc;

    for (int i = 0; i < 256; i++) {
      crc = i;
      for (int j = 0; j < 8; j++) {
        crc >>= 1;
        if (crc & 1) {
          crc ^= CRC_MAGIC;
        }
      }
    }

    CRC_HASH_CACHED = true;
  }

  uint32_t crc = 0xFFFFFFFF;
  char c;
  std::streampos pos_initial = input.tellg();

  while ((c = input.get()) != EOF) {
    crc = (crc >> 8) ^ CRC_HASH[(c ^ crc) & 0xFF];
  }

  input.seekg(pos_initial);
  return ~crc;
}

} // namespace fileutils
} // namespace utils
} // namespace screenspacemanager