#include <gtest/gtest.h>
#include <critter/GameObject.hpp>

// verify successful instantiation of trivially derived class
// verify transform matrix accuracy
// add child and verify that child matrix is correct

using ::monkeysworld::critter::GameObject;

class DummyGameObject : public GameObject {
  public:
    void Draw() override {
      // do nothing
    }
};

TEST(GameObjectTests, CreateGameObject) {
  DummyGameObject object;
  ASSERT_EQ(nullptr, object.GetParent());
  
}