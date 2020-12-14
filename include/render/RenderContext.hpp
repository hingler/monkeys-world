#ifndef RENDER_CONTEXT_H_
#define RENDER_CONTEXT_H_

#include <critter/Object.hpp>

#include <memory>

namespace monkeysworld {
namespace render {

/**
 *  Passed to components so that they can render themselves.
 *  Should the ctor handle visiting?
 *  Yeah I think that would be pretty cool actually
 *  We pass it the root GameObject and it generates everything
 */ 
class RenderContext {
 public:
  /**
   *  Creates a render context by descending through
   *  the scene and generating structures as necessary.
   */ 
  RenderContext(std::shared_ptr<critter::Object> root);

};

}
}

#endif
