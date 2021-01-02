// create hierarchies containing 0, 1, 2+ active cameras
// simply ensure that they return a result in the latter two cases

#include <gtest/gtest.h>
#include <critter/visitor/ActiveCameraFindVisitor.hpp>
#include <critter/GameCamera.hpp>
#include <critter/Empty.hpp>

#include <memory>

using monkeysworld::critter::Empty;
using monkeysworld::critter::GameCamera;
using monkeysworld::critter::visitor::ActiveCameraFindVisitor;

#define TEST_ID1 1024

TEST(ActiveCameraFindVisitorTests, CreateVisitor) {
  ActiveCameraFindVisitor v;
  auto e = std::make_shared<Empty>(nullptr);
  auto c = std::make_shared<GameCamera>(nullptr);

  c->SetId(TEST_ID1);
  c->SetActive(false);

  e->AddChild(c);
  e->Accept(v);
  auto cam_find = v.GetActiveCamera();
  ASSERT_EQ(nullptr, cam_find.get());
}

TEST(ActiveCameraFindVisitorTests, FindActiveCamera) {
  ActiveCameraFindVisitor v;
  
  auto e = std::make_shared<Empty>(nullptr);
  auto c = std::make_shared<GameCamera>(nullptr);

  c->SetId(TEST_ID1);
  c->SetActive(true);

  e->AddChild(c);
  e->Accept(v);
  auto cam_find = v.GetActiveCamera();
  ASSERT_NE(nullptr, cam_find.get());
  ASSERT_EQ(TEST_ID1, cam_find->GetId());

  v.Clear();
  ASSERT_EQ(nullptr, v.GetActiveCamera().get());
}

TEST(ActiveCameraFindVisitorTests, FindActiveCameraInHierarchy) {
  ActiveCameraFindVisitor v;
  
  auto e1 = std::make_shared<Empty>(nullptr);
  auto e2 = std::make_shared<Empty>(nullptr);
  auto e3 = std::make_shared<Empty>(nullptr);

  auto c = std::make_shared<GameCamera>(nullptr);

  c->SetId(TEST_ID1);
  c->SetActive(true);

  e1->AddChild(e2);
  e2->AddChild(e3);
  e2->AddChild(c);

  e1->Accept(v);
  auto cam_find = v.GetActiveCamera();
  ASSERT_NE(nullptr, cam_find.get());
  ASSERT_EQ(TEST_ID1, c->GetId());
}