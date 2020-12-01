#ifndef PORT_AUDIO_EXCEPTION_H_
#define PORT_AUDIO_EXCEPTION_H_

#include <string>
#include <stdexcept>

namespace monkeysworld {
namespace audio {
namespace exception {

class PortAudioException : public std::runtime_error {
 public:
  PortAudioException(const std::string& arg) : runtime_error(arg) {}
  PortAudioException(const char* arg) : runtime_error(arg) {}
};

}
}
}

#endif