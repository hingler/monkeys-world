#include <critter/Skybox.hpp>

namespace monkeysworld {
namespace critter {

using engine::Context;
using shader::CubeMap;

Skybox::PositionPacket::Bind() {
  glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Skybox::PositionPacket), (void*)0);
  glEnableVertexAttribArray(0);
}

Skybox::Skybox(Context* ctx) : GameObject(ctx), mat_(ctx) {
  // VERTICES
  mesh_.AddVertex({ glm::vec3(-1, -1, -1 ) });
  mesh_.AddVertex({ glm::vec3(-1, -1,  1 ) });
  mesh_.AddVertex({ glm::vec3(-1,  1, -1 ) });
  mesh_.AddVertex({ glm::vec3(-1,  1,  1 ) });
  mesh_.AddVertex({ glm::vec3( 1, -1, -1 ) });
  mesh_.AddVertex({ glm::vec3( 1, -1,  1 ) });
  mesh_.AddVertex({ glm::vec3( 1,  1, -1 ) });
  mesh_.AddVertex({ glm::vec3( 1,  1,  1 ) });

  // POLYGONS
  mesh_.AddPolygon(1, 2, 3);
  mesh_.AddPolygon(2, 4, 3);
  mesh_.AddPolygon(1, 5, 3);
  mesh_.AddPolygon(5, 7, 3);
  mesh_.AddPolygon(7, 5, 8);
  mesh_.AddPolygon(5, 6, 8);
  mesh_.AddPolygon(1, 2, 5);
  mesh_.AddPolygon(5, 2, 6);
  mesh_.AddPolygon(8, 6, 2);
  mesh_.AddPolygon(6, 4, 8);
  mesh_.AddPolygon(7, 3, 4);
  mesh_.AddPolygon(3, 4, 8);

  cube_map_ = std::shared_ptr<CubeMap>(nullptr);
}

Skybox::Skybox(Context* ctx, std::shared_ptr<CubeMap> c) : Skybox(ctx), mat_(ctx) {
  SetCubeMap(c);
}

void Skybox::SetCubeMap(std::shared_ptr<CubeMap> c) {
  cube_map_ = c;
}

void Skybox::PrepareAttributes() {
  mesh_.PointToVertexAttribs();
}

void Skybox::RenderMaterial(const engine::RenderContext& rc) {
  auto cam_data = rc.GetActiveCamera();
  mat_.SetCameraView(cam_data.view_matrix);
  mat_.SetCameraPersp(cam_data.persp_matrix);
  if (cube_map_) {
    mat_.SetCubeMap(cube_map_->GetCubeMapDescriptor());
  }

  mat_.UseMaterial();
  Draw();
}

void Skybox::Draw() {
  glDrawElements(GL_TRIANGLES, mesh_.GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
}

}
}