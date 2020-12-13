#ifndef BASE_ENGINE_H_
#define BASE_ENGINE_H_

#include <engine/Scene.hpp>

namespace monkeysworld {
namespace engine {
namespace baseengine {

// function to start the engine loop with some premade scene
void GameLoop(std::shared_ptr<Scene> scene);

// various functions which help push the engine along

}
}
}

#endif