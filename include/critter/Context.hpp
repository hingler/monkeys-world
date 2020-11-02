#ifndef CONTEXT_H_
#define CONTEXT_H_

namespace monkeysworld {
namespace critter {

/**
 *  The `Context` class intends to provide access to components for which a single instance will be associated
 *  with execution at a time.
 * 
 *  For instance, handlers for audio, file loaders, global event broadcasters, etc.
 *  All resources here *should* be threadsafe.
 */ 
class Context {
  // ctor, or get/set?
  // store shared ptr
  // other context objects can "get" from last context (probably a good idea to port over like audio handler for inst)
  // use a builder pattern to create the context
    // use premade assets if available, or construct from defaults if not
};

}
}

#endif