#ifndef FILE_UTILS_H_
#define FILE_UTILS_H_

#include <cinttypes>
#include <fstream>

namespace screenspacemanager {
namespace utils {
namespace fileutils {

/**
 *  A series of stateless functions associated with file R/W.
 */ 

/**
 *  Calculates the CRC Hash of the provided file, from `offset` forward.
 *  @param input - the istream we are reading from.
 *  @param offset - absolute offset, relative to ios_base::beg, to start reading from.
 */ 
uint32_t CalculateCRCHash(std::istream& input, std::streamoff offset);

} // namespace fileutils
} // namespace utils
} // namespace screenspacemanager

#endif