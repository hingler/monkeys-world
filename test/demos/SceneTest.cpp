// test for game engine methods. creates a basic scene

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <engine/BaseEngine.hpp>

#include <engine/Scene.hpp>

// create everything inline -- its pretty lazy

using ::monkeysworld::engine::Scene;
using ::monkeysworld::critter::Object;

/**
 *  Simple test scene
 */ 
class TestScene : public Scene {
 public:
  void Initialize() override {
    // TODO: USE THE RAT! https://sketchfab.com/3d-models/rat-847629266c0f442da74fb132f46f3baf

  }

  std::shared_ptr<Object> GetGameObjectRoot() {
    return std::shared_ptr<Object>();
  }
 private:
  std::shared_ptr<Object> game_object_root_;  // game object root lol
};

void main(int argc, char** argv) {

}
