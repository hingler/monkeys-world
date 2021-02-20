#ifndef AUDIO_BUFFER_PACKET_H_
#define AUDIO_BUFFER_PACKET_H_

#include <atomic>

/**
 *  Q: Do we need to keep track of bytes allocated by this method, separately from bytes which can actually be read?
 *  A: We already assume that our buffer is being read from no more than a single thread -- that plays into the design of the class thus far.
 *     Based on this guarantee, this packet will only be read from a single thread -- thus, requrest will never overlap,
 *     and all we have to worry about is the bytes read.
 */ 

namespace monkeysworld {
namespace audio {

struct AudioBufferPacket {
  float* left;
  float* right;
  const uint64_t capacity;
};

}
}

#endif