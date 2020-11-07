#ifndef CONTEXT_H_
#define CONTEXT_H_

#include <file/CachedFileLoader.hpp>

#include <memory>

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
  // something for borrowing components
  // create a context builder which can be used to pass in components?
  // data class which contains dependencies
  // and which can be fetched from the struct

  // this should make it easier to add components and shit

  // note: the CONTEXT should be detached from state. this thing should exist from start to finish,
  // and should be closely tied to system components which require a rooted
 public:
  /**
   *  Default constructor which attempts to initialize all fields.
   */ 
  Context();

  // the following functions return some higher level component

  // add timer + timed event handler

  // file loader
  const std::shared_ptr<file::CachedFileLoader> GetCachedFileLoader();
  // NOTE: This should DEFINITELY be provided by the context object --
  //       however, we need to swap it out when loading into a new "context"

  //       Creating a per-instance context would work, however there are also cases
  //       when we wouldn't want to abandon old context constants,
  //       for instance we would probably want to preserve an audio-loading component
  //       across calls.

  //       One soln: reframe the context object into a "context local" and a "context global"
  //       "Context global" is composited into "Context local" which then passes the resources
  //       of "Context global" to the client as necessary

  //       Soln: make the context an abstract
  //       Use a "global state" object as a ctor for new contexts
  //       Ergo, new contexts only contain information which is relevant to a single instance
  //          - this almost seems to coincide better with a notion of a "scene"
  //          - to our application, this should be handled silently
  //          - however, we should be able to customize the state of the context
  //            behind the scenes.
  
  //       Separate notion of global vs. local scope for contexts; i.e. components we wouldn't
  //       really want (or need) to set up when we jump between, well, contexts.

  //       We would definitely want to keep our file loader scoped locally, allowing the backend
  //       to swap things out as it deems fit

  //       However, we don't want carry over

  //       Better yet: there's a notion of global dependencies, and local components.
  //       It would be a pain to wipe a global component and then pass it in, so we design it
  //       such that the local component cleans itself up by making calls to the global component
  //       Then, the new context makes a new local space, and starts making calls to the global
  //       component.

  //       The user should see the context as a provider. We make calls to it, it does things, and
  //       it cleans up at the end -- that's not our responsibility, it should do what we want it to.

  //       We should be expected to clean up that which we allocate.

  //       Thus:
  //         - "Local" components associated with a context manage creation and cleanup.
  //         - This is facilitated by calls to a "global" context, which is configured on ctor call.
  //         - Each context therefore runs off of local components which are completely fresh.

  //       Some things to worry about later:
  //         - Can this be done seamlessly? If not, how can we pass off a context?

  //       Since i'm borrowing "contexts" from android: Android has a notion of appl and activity contexts.
  //       How will this be facilitated?
  //       i.e. how do we discern from local and global scope when dealing with our context object?

  //       My thought: rather than context-specific, this is component-specific.
  //       AKA we make a component which is specifically designed to put together a data store
  //       for our contexts, within some scope (probably global since its the next step up)

  //       components are designed to make calls to global entities, and thus it's their job to decide
  //       how to store information in those global entities.

  //       however, that particular instance dies when we switch contexts.

  //       what is a context switch?
  //       probably gonna be similar to switching between "screens"
  //       hitting the "start" button is probably a context switch
  //       going from level select to loading screen is probably a context switch
  //       scoreboards or settings could be a context switch
  //       if we want to make it smooth, we need a context object which stores that data globally
  //       thus ensuring we can transition between points

  //       ex: let's say we wanted to transition from a menu to a high score screen with an effect
  //       we could use a context component to save our framebuffer somewhere accessible
  //       then we would context switch
  //       the new context associated with the high score table would then read the framebuffer
  //       from a app level store and display it (no draw calls should be called between then and now)
  //       and we animate from the new context
 private:
  std::shared_ptr<file::CachedFileLoader> file_loader_;
};

//        future problem: smoothly passing the baton to other components
//                        and passing information between contexts

//                        i.e. what if i want some sound to fade into the next context?
//                        i think this should be application specific, and having a global/local
//                        scope facilitates this -- for a sound: we could tell the sound lib to
//                        shut up, or we could let it play out, or we could tell it to fade out
//                        a sound, or whatever -- the global object lives on, so just tell it
//                        what to do when transitioning

//                        the one thing we hold the keys on is the render context --
//                        we can add some default behavior to save a framebuffer somewhere and
//                        keep drawing it during a context switch
//                        actually it would be good practice in general to save the last FB
//                        so that we can do something with it

//  basically: context is local, it's constructed from global resources.
//  it enables access to those global resources, and ensures that resources associated with a single
//  "real" context are no longer relevant outside of it

} // namespace critter
} // namespace monkeysworld

#endif