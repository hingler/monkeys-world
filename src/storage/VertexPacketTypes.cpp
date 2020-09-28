#include "storage/VertexPacketTypes.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace screenspacemanager {
namespace storage {

void VertexPacket2D::Bind() {
  // position data (position 0)
  glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(VertexPacket2D), (void*)0);
  glEnableVertexAttribArray(0);

  // texcoord data (position 1)
  glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexPacket2D), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);
}

// enablevertexattribarray for all kinda sucks maybe?
void VertexPacket3D::Bind() {
  // position data (position 0)
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexPacket3D), (void*)0);
  glEnableVertexAttribArray(0);

  // normal data (position 1)
  glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(VertexPacket3D), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // tex data (position 2)
  glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(VertexPacket3D), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);
}

};  // namespace storage
};  // namespace screenspacemanager