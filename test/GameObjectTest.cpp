#include <gtest/gtest.h>
#include <critter/GameObject.hpp>

#include <glm/gtc/matrix_transform.hpp>

// verify successful instantiation of trivially derived class
// verify transform matrix accuracy
// add child and verify that child matrix is correct

using ::monkeysworld::critter::GameObject;


class DummyGameObject : public GameObject {
  public:
    void PrepareAttributes() override {
      // do nothing
    }

    void RenderMaterial() override {
      // also do nothing
    }
};

TEST(GameObjectTests, CreateGameObject) {
  DummyGameObject object;
  ASSERT_EQ(nullptr, object.GetParent());
  ASSERT_EQ(nullptr, object.GetChild(255));
}

TEST(GameObjectTests, NestObject) {
  std::shared_ptr<DummyGameObject> parent = std::make_shared<DummyGameObject>();
  std::shared_ptr<DummyGameObject> child = std::make_shared<DummyGameObject>();
  std::shared_ptr<GameObject> cast_child = std::dynamic_pointer_cast<GameObject>(child);
  parent->AddChild(cast_child);
  ASSERT_EQ(parent.get(), child->GetParent());
  ASSERT_EQ(child.get(), parent->GetChild(child->GetId()));
}

TEST(GameObjectTests, VerifyTransformations) {
  DummyGameObject object;
  object.SetPosition(glm::vec3(0, 0, 2));
  object.SetRotation(glm::vec3(0, 0, 1));

  glm::mat4 object_rot = object.GetTransformationMatrix();

  glm::mat4 actual_transform = glm::mat4(1.0);
  // our object scales, then rotates, then transforms
  actual_transform = glm::translate(actual_transform, glm::vec3(0, 0, 2));
  actual_transform = glm::rotate(actual_transform, 1.0f, glm::vec3(0, 0, 1));

  // uncomment to double check matrix values
  // for (int i = 0; i < 4; i++) {
  //   for (int j = 0; j < 4; j++) {
  //     std::cout << object_rot[i][j] << ", ";
  //   }
  // }

  // std::cout << std::endl;

  // for (int i = 0; i < 4; i++) {
  //   for (int j = 0; j < 4; j++) {
  //     std::cout << actual_transform[i][j] << ", ";
  //   }
  // }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ASSERT_NEAR(object_rot[i][j], actual_transform[i][j], 0.001);
    }
  }

  object.SetScale(glm::vec3(2, 2, 2));
  object_rot = object.GetTransformationMatrix();
  actual_transform = glm::scale(actual_transform, glm::vec3(2));

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ASSERT_NEAR(object_rot[i][j], actual_transform[i][j], 0.001);
    }
  }
}