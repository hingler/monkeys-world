#include <shader/Canvas.hpp>
#include <model/Mesh.hpp>

#include <shader/materials/FillMaterial.hpp>

#include <mutex>

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

  // for ensuring geometry need not be reinstanced
  model::Mesh<storage::VertexPacket2D> geom_cache;
  std::mutex geom_lock;
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
  glm::vec2 normal = glm::vec2(-direction.y, direction.x);
  // convert start and end to screenspace coordinates

  glm::vec2 fb_dims(fb_->GetDimensions());
  fb_dims /= 2;
  end /= fb_dims;
  start /= fb_dims;
  end -= glm::vec2(1);
  start -= glm::vec2(1);
  normal /= fb_dims;
  VertexPacket2D temp;

  {
    std::unique_lock<std::mutex> lock(geom_lock);
    geom_cache.Clear();

    {
      temp.position = (start - normal * (thickness / 2));
      geom_cache.AddVertex(temp);
      temp.position = (start + normal * (thickness / 2));
      geom_cache.AddVertex(temp);
      temp.position = (end - normal * (thickness / 2));
      geom_cache.AddVertex(temp);
      temp.position = (end + normal * (thickness / 2));
      geom_cache.AddVertex(temp);
      geom_cache.AddPolygon(0, 2, 1);
      geom_cache.AddPolygon(1, 2, 3);
    }

    fill_mat->SetColor(color);
    fill_mat->UseMaterial();
    geom_cache.PointToVertexAttribs();
    glDrawElements(GL_TRIANGLES, static_cast<int>(geom_cache.GetIndexCount()), GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
  }
}

}
}