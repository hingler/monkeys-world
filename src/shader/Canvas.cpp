#include <shader/Canvas.hpp>
#include <model/Mesh.hpp>

#include <shader/materials/FillMaterial.hpp>

namespace monkeysworld {
namespace shader {

using model::Mesh;
using storage::VertexPacket2D;
using materials::FillMaterial;

namespace {
  // shaders used by canvas
  std::shared_ptr<FillMaterial> fill_mat;

  std::mutex build_shaders_mtx;
  std::atomic_bool shaders_built = false;
}

Canvas::Canvas(std::shared_ptr<Framebuffer> framebuffer) {
  fb_ = framebuffer;
  // quick lock-free check (it's kinda gross but whatever)
  if (!shaders_built) {
    std::unique_lock<std::mutex> lock(build_shaders_mtx);
    if (!shaders_built) {
      fill_mat = std::make_shared<FillMaterial>();
    }

    shaders_built = true;
  }
}

void Canvas::DrawLine(glm::vec2 start, glm::vec2 end, float thickness, glm::vec4 color) {
  
  fb_->BindFramebuffer();
  glm::vec2 direction = glm::normalize(end - start);
  // length of 1px -- need to convert that to screenspace
  glm::vec2 normal = glm::vec2(direction.y, direction.x);
  // convert start and end to screenspace coordinates

  glm::vec2 fb_dims(fb_->GetDimensions());
  fb_dims /= 2;
  end /= fb_dims;
  start /= fb_dims;
  end -= glm::vec2(1);
  start -= glm::vec2(1);
  normal /= fb_dims;
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

  fill_mat->SetColor(color);
  fill_mat->UseMaterial();
  line_geom.PointToVertexAttribs();
  glDrawElements(GL_TRIANGLES, line_geom.GetIndexCount(), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
}

}
}