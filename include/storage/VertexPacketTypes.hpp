#ifndef VERTEX_PACKET_TYPES_H_
#define VERTEX_PACKET_TYPES_H_

#include <glm/glm.hpp>

/**
 *  A vertex packet containing two position dimensions and two texture dimensions.
 *  Intended for UI geometry, for instance on-screen text.
 */ 
struct VertexPacket2D {
  // 2D position
  glm::vec2 position;

  // 2D tex coords
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
  glm::vec3 position;
  glm::vec3 normals;
  glm::vec2 coords;

  /**
   *  Points to own vertex attributes.
   */ 
  static void Bind();
};



#endif  // VERTEX_PACKET_TYPES_H_