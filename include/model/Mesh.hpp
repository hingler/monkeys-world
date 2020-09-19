#ifndef MESH_H_
#define MESH_H_

namespace screenspacemanager {
namespace model {

  // mesh -> model -> game-specific handling

  /**
   *  A mesh constitutes some piece of geometry which contributes, or constitutes, a full-fledged model.
   *  It contains all static resources required to draw the mesh, which includes GLSL attributes as well as textures.
   */ 
  class Mesh {
    /**
     *  figure out some basic features
     *    - loading a mesh from file seems obvious
     *    - merge meshes together?
     *      - is rendering something which **needs** to be done on a per-mesh basis?
     *      - probably not, but leave it off for now
     *    - figure out how textures are linked, and then provide those
     *      - need a data thing for those as well
     *    - allow for some uniform editing (attributes will be locked in the vertex data)
     *    - provide some method which binds this mesh's contents as active
     *      - these can then be called by the `Model` to draw it on-screen with all uniforms set
     *      - 
     *    - enable access to the vertex data (see that class for another note lol)
     */ 
  };

};  // namespace gl
};  // namespace screenspacemanager

#endif  // MESH_H_