#include <critter/Skybox.hpp>

namespace monkeysworld {
namespace critter {

using engine::Context;
using shader::CubeMap;

void Skybox::PositionPacket::Bind() {
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
  mesh_.AddPolygon(0, 1, 2);
  mesh_.AddPolygon(2, 3, 1);
  mesh_.AddPolygon(0, 2, 4);
  mesh_.AddPolygon(4, 6, 2);
  mesh_.AddPolygon(6, 4, 7);
  mesh_.AddPolygon(4, 5, 7);
  mesh_.AddPolygon(0, 1, 4);
  mesh_.AddPolygon(4, 1, 5);
  mesh_.AddPolygon(1, 3, 5);
  mesh_.AddPolygon(5, 7, 3);
  mesh_.AddPolygon(2, 3, 6);
  mesh_.AddPolygon(6, 7, 3);

  cube_map_ = std::shared_ptr<CubeMap>(nullptr);
}

Skybox::Skybox(Context* ctx, std::shared_ptr<const CubeMap> c) : Skybox(ctx) {
  SetCubeMap(c);
}

void Skybox::SetCubeMap(std::shared_ptr<const CubeMap> c) {
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