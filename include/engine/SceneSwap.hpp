#ifndef SCENE_SWAP_H_
#define SCENE_SWAP_H_

#include <file/CachedFileLoader.hpp>

#include <condition_variable>
#include <memory>
#include <mutex>

namespace monkeysworld {
namespace engine {

class Context;

/**
 *  Used to view the state of a scene as it loads.
 */ 
class SceneSwap {

 public:
  /**
   *  Creates a new SceneSwap instance.
   *  @param ctx - the context which will eventually be loaded on swap.
   */ 
  SceneSwap(std::shared_ptr<Context> ctx,
            std::shared_ptr<std::mutex> mutex,
            std::shared_ptr<std::condition_variable> load_cv);

  /**
   *  @returns how much of the scene has been loaded.
   */ 
  file::loader_progress GetLoaderProgress();

  /**
   *  Swaps scenes if the next context is ready to be used.
   *  Otherwise, waits until the next scene is initialized, then swaps.
   */ 
  void Swap();

  /**
   *  @returns true if we're ready to swap scenes, false otherwise.
   */ 
  bool IsSwapReady();

 private:
  std::shared_ptr<Context> ctx_;
  std::shared_ptr<std::mutex> mutex_;
  std::shared_ptr<std::condition_variable> load_cv_;
  bool swap_ready_;

};

}
}

#endif