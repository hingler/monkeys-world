// currently just spotlights
// create a few hierarchies containing lights
// see what gets returned after the light visitor visits them all

#include <gtest/gtest.h>
#include <critter/visitor/LightVisitor.hpp>
#include <shader/light/SpotLight.hpp>
#include <critter/Empty.hpp>

#include <algorithm>
#include <memory>

using monkeysworld::critter::Empty;
using monkeysworld::shader::light::SpotLight;
using monkeysworld::critter::visitor::LightVisitor;

#define TEST_ID 1024

TEST(LightVisitorTests, CreateLightVisitor) {
  // create single spot light
  // set id
  // visit
  // ensure that new ID matches old
  LightVisitor visitor;
  auto e = std::make_shared<Empty>(nullptr);
  auto s = std::make_shared<SpotLight>(nullptr);

  s->SetId(TEST_ID);

  e->AddChild(s);

  e->Accept(visitor);

  auto list = visitor.GetSpotLights();

  ASSERT_EQ(1, list.size());
  auto light_check = list[0];
  ASSERT_EQ(TEST_ID, light_check->GetId());
}

#define TEST_ID_A 2048
#define TEST_ID_B 2049

TEST(LightVisitorTests, MultipleLightHierarchy) {
  LightVisitor visitor;

  auto e1 = std::make_shared<Empty>(nullptr);
  auto e2 = std::make_shared<Empty>(nullptr);
  auto e3 = std::make_shared<Empty>(nullptr);

  auto l1 = std::make_shared<SpotLight>(nullptr);
  auto l2 = std::make_shared<SpotLight>(nullptr);

  l1->SetId(TEST_ID_A);
  l2->SetId(TEST_ID_B);

  e2->AddChild(l1);
  e3->AddChild(l2);
  e1->AddChild(e2);
  e2->AddChild(e3);

  e1->Accept(visitor);
  auto list = visitor.GetSpotLights();

  // ensure one light has 2048, and one has 2049
  ASSERT_EQ(2, list.size());
  ASSERT_TRUE(std::find(list.begin(), list.end(), l1) != list.end());
  ASSERT_TRUE(std::find(list.begin(), list.end(), l2) != list.end());
}