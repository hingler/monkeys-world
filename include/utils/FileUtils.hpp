#ifndef FILE_UTILS_H_
#define FILE_UTILS_H_

#include <cinttypes>
#include <fstream>
#include <iostream>

namespace monkeysworld {
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

/**
 *  Writes to a file as bytes.
 *  @param <input_type>: The type being written.
 *  @param output - the output stream being written to.
 *  @param data - the data being written.
 */ 
template <typename input_type>
void WriteAsBytes(std::ostream& output, input_type data) {
  output.write(reinterpret_cast<char*>(&data), sizeof(input_type));
}

/**
 *  Reads bytes from a file to a return object.
 *  @param <output_type>: The type being read.
 *  @param input - the stream being read from.
 */ 
template <typename output_type>
output_type ReadAsBytes(std::istream& input) {
  output_type output;
  input.read(reinterpret_cast<char*>(&output), sizeof(output_type));
  return output;
}

} // namespace fileutils
} // namespace utils
} // namespace monkeysworld

#endif