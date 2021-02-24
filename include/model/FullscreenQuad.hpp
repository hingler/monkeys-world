#ifndef FULLSCREEN_QUAD_H_
#define FULLSCREEN_QUAD_H_

#include <engine/Context.hpp>
#include <model/Mesh.hpp>
#include <storage/VertexPacketTypes.hpp>

namespace monkeysworld {
namespace model {

/**
 *  The FullscreenQuad contains a mesh which, in NDC, spans the entire width of the window.
 * 
 *  This can be quickly constructed and utilized for drawing quads which occupy the entirety
 *  of a framebuffer.
 */ 
class FullscreenQuad {
 public:
  /**
   *  Creates a new FullscreenQuad.
   */ 
  FullscreenQuad();

  /**
   *  Points to, and draws, this fullscreen quad.
   */ 
  void PointAndDraw();
 private:
  Mesh<storage::VertexPacket2D> quad_mesh_;
};

}
}

#endif