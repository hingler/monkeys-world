#include "storage/VertexPacketTypes.hpp"

#include <boost/log/trivial.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace monkeysworld {
namespace storage {

void VertexPacket2D::Bind() {
  // position data (position 0)
  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(VertexPacket2D), (void*)0);
  glEnableVertexAttribArray(0);

  // texcoord data (position 1)
  glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexPacket2D), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

void VertexPacket3D::Bind() {
  // position data (position 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPacket3D), (void*)0);
  glEnableVertexAttribArray(0);
  

  // tex data (position 1)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPacket3D), (void*)(3 * sizeof(float)));
  

  // normal data (position 2)
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPacket3D), (void*)(5 * sizeof(float)));
  
}

};  // namespace storage
};  // namespace monkeysworld