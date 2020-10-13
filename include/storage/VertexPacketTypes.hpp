#ifndef VERTEX_PACKET_TYPES_H_
#define VERTEX_PACKET_TYPES_H_

#include <glm/glm.hpp>

namespace screenspacemanager {
namespace storage {

/**
 *  A vertex packet containing two position dimensions and two texture dimensions.
 *  Intended for UI geometry, for instance on-screen text.
 */ 
struct VertexPacket2D {
  // 2D position (location = 0)
  glm::vec2 position;

  // 2D tex coords (location = 1)
  glm::vec2 texcoords;

  /**
   *  Points to own vertex attributes.
   */ 
  static void Bind();
};

/**
 *  Vertex packet more typically used for representing 3D objects.
 *  Contains three position dimensions, two texture dimensions, and three normal dimensions.
 */ 
struct VertexPacket3D {
  // 3D position (location = 0)
  glm::vec3 position;

  // 2D texcoords (location = 1)
  glm::vec2 coords;
  
  // 3D normals (location = 2)
  glm::vec3 normals;


  /**
   *  Points to own vertex attributes.
   */ 
  static void Bind();
};

};  // namespace storage
};  // namespace screenspacemanager

#endif  // VERTEX_PACKET_TYPES_H_