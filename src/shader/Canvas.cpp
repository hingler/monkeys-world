#include <shader/Canvas.hpp>
#include <model/Mesh.hpp>

#include <shader/materials/FillMaterial.hpp>

namespace monkeysworld {
namespace shader {

using model::Mesh;
using storage::VertexPacket2D;
using materials::FillMaterial;

namespace {
  FillMaterial fill_mat;
}

Canvas::Canvas(std::shared_ptr<Framebuffer> framebuffer) {
  fb_ = framebuffer;
}

void Canvas::DrawLine(glm::vec2 start, glm::vec2 end, float thickness, glm::vec4 color) {
  
  fb_->BindFramebuffer();
  glm::vec2 direction = glm::normalize(end - start);
  glm::vec2 normal = glm::vec2(direction.y, direction.x);
  // convert start and end to screenspace coordinates

  glm::vec2 fb_dims(fb_->GetDimensions());
  end /= fb_dims;
  start /= fb_dims;
  Mesh<VertexPacket2D> line_geom;
  VertexPacket2D temp;
  {
    temp.position = (start - normal * (thickness / 2));
    line_geom.AddVertex(temp);
    temp.position = (start + normal * (thickness / 2));
    line_geom.AddVertex(temp);
    temp.position = (end - normal * (thickness / 2));
    line_geom.AddVertex(temp);
    temp.position = (end + normal * (thickness / 2));
    line_geom.AddVertex(temp);
  }

  fill_mat.SetColor(color);
  fill_mat.UseMaterial();
  line_geom.PointToVertexAttribs();
  glDrawElements(GL_TRIANGLES, line_geom.GetIndexCount(), GL_UNSIGNED_INT, (void*)0);
}

}
}