#include <model/FullscreenQuad.hpp>

namespace monkeysworld {
namespace model {

FullscreenQuad::FullscreenQuad() : quad_mesh_() {
  storage::VertexPacket2D temp;
  {
    temp.position = glm::vec2(-1, -1);
    temp.texcoords = glm::vec2(0, 0);
    quad_mesh_.AddVertex(temp);

    temp.position = glm::vec2(1, -1);
    temp.texcoords = glm::vec2(1, 0);
    quad_mesh_.AddVertex(temp);

    temp.position = glm::vec2(1, 1);
    temp.texcoords = glm::vec2(1, 1);
    quad_mesh_.AddVertex(temp);

    temp.position = glm::vec2(-1, 1);
    temp.texcoords = glm::vec2(0, 1);
    quad_mesh_.AddVertex(temp);

    quad_mesh_.AddPolygon(0, 1, 2);
    quad_mesh_.AddPolygon(2, 3, 0);
  }
}

void FullscreenQuad::PointAndDraw() {
  quad_mesh_.PointToVertexAttribs();
  glDrawElements(GL_TRIANGLES, static_cast<int>(quad_mesh_.GetIndexCount()), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
}

}
}