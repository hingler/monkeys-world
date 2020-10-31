#ifndef GL_DEBUG_H_
#define GL_DEBUG_H_

#include <glad/glad.h>
#include <boost/log/trivial.hpp>

#include <string>

namespace monkeysworld {
namespace shader {
namespace gldebug {

const char* GetSourceType(GLenum flag) {
  switch (flag) {
    case GL_DEBUG_SOURCE_API:
      return "OGL API Call";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "window API Call";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "shader compiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "some other application";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "user";
    case GL_DEBUG_SOURCE_OTHER:
      return "some other source";
    default:
      return "UNKNOWN";
  }
}

const char* GetMessageType(GLenum type) {
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      return "error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "function call marked as depreceated";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "invocation triggering undefined behavior";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "non-portable function call";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "potential performance issue";
    case GL_DEBUG_TYPE_MARKER:
      return "misc. annotation";
    case GL_DEBUG_TYPE_PUSH_GROUP:
      return "user-defined debug group creation";
    case GL_DEBUG_TYPE_POP_GROUP:
      return "user-defined debug group deletion";
    case GL_DEBUG_TYPE_OTHER:
      return "some other type";
    default:
      return "UNKNOWN";
  }
}

const char* GetSeverity(GLenum severity) {
  switch(severity) {
    case GL_DEBUG_SEVERITY_HIGH:
      return "ERROR";
    case GL_DEBUG_SEVERITY_MEDIUM:
      return "WARNING";
    case GL_DEBUG_SEVERITY_LOW:
      return "INFO";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      return "DEBUG";
    default:
      return "UNKNOWN";
  }
}

void glcallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userdata) {
  BOOST_LOG_TRIVIAL(debug) << "GL Debug Output ~~ ["
                           << GetSeverity(severity) 
                           << "] From "
                           << GetSourceType(source)
                           << " - "
                           << GetMessageType(type)
                           << ": "
                           << std::string(message, length);
}

void SetupGLDebug() {
  BOOST_LOG_TRIVIAL(debug) << "enable output";
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(&glcallback, NULL);
}

} // namespace gldebug
} // namespace shader
} // namespace monkeysworld

#endif  // GL_DEBUG_H_