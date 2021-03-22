#include <gtest/gtest.h>
#include <shader/color/Gradient.hpp>

using ::monkeysworld::shader::color::Gradient;

TEST(GradientTests, DefaultGradient) {
  Gradient g;
  auto stops = g.GetStops();
  ASSERT_EQ(1, stops.size());
  ASSERT_NEAR(0.5f, stops[0].stop, 0.0001f);
}

TEST(GradientTests, CreateGradient) {
  Gradient g;
  g.AddStop(glm::vec4(0.2), -1.0f);
  g.AddStop(glm::vec4(0.8), 1.0f);
  g.AddStop(glm::vec4(0.1), 0.5f);

  auto stops = g.GetStops();
  ASSERT_EQ(3, stops.size());

  ASSERT_NEAR(stops[0].col.r, 0.2f, 0.0001f);
  ASSERT_NEAR(stops[0].col.g, 0.2f, 0.0001f);
  ASSERT_NEAR(stops[0].col.b, 0.2f, 0.0001f);
  ASSERT_NEAR(stops[0].col.a, 0.2f, 0.0001f);

  ASSERT_NEAR(stops[1].col.r, 0.1f, 0.0001f);
  ASSERT_NEAR(stops[1].col.g, 0.1f, 0.0001f);
  ASSERT_NEAR(stops[1].col.b, 0.1f, 0.0001f);
  ASSERT_NEAR(stops[1].col.a, 0.1f, 0.0001f);

  ASSERT_NEAR(stops[2].col.r, 0.8f, 0.0001f);
  ASSERT_NEAR(stops[2].col.g, 0.8f, 0.0001f);
  ASSERT_NEAR(stops[2].col.b, 0.8f, 0.0001f);
  ASSERT_NEAR(stops[2].col.a, 0.8f, 0.0001f);

  ASSERT_NEAR(stops[0].stop, 0.0f, 0.0001f);
  ASSERT_NEAR(stops[1].stop, 0.5f, 0.0001f);
  ASSERT_NEAR(stops[2].stop, 1.0f, 0.0001f);

  g.ClearStops();
  stops = g.GetStops();
  ASSERT_EQ(1, stops.size());
  ASSERT_NEAR(0.5f, stops[0].stop, 0.0001f);
}