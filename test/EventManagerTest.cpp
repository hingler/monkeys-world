#include <gtest/gtest.h>
#include <input/WindowEventManager.hpp>
#include <utils/IDGenerator.hpp>

using monkeysworld::input::WindowEventManager;

/**
 *  Subclass to avoid GLFW commands. We'll have to spoof input.
 */ 
class TestingEventManager : public WindowEventManager {
 public:
  TestingEventManager() : WindowEventManager() {}
};

class EventManagerTests : public ::testing::Test {
 public:
  std::shared_ptr<TestingEventManager> event_mgr;
 protected:
  void SetUp() override {
    event_mgr = std::make_shared<TestingEventManager>();
  }
};


TEST_F(EventManagerTests, CreateEvent) {
  int good = 0;
  auto test_lambda = [&good](int, int, int) {
    good = 1;
  };

  uint64_t event = event_mgr->RegisterKeyListener(0, test_lambda);
  event_mgr->ProcessKeyEvent(NULL, 0, 0, 0, 0);
  ASSERT_EQ(1, good);

  good = 0;
  event_mgr->ProcessKeyEvent(NULL, 15, 0, 0, 0);
  ASSERT_EQ(0, good);
}

TEST_F(EventManagerTests, CreateMultipleEventsOnSameKey) {
  int sogob = 0;
  int paric = 0;
  auto lambda_sogob = [&sogob](int, int, int) {
    sogob = 1;
  };

  auto lambda_paric = [&paric](int, int, int) {
    paric = 1;
  };

  uint64_t event_sogob = event_mgr->RegisterKeyListener(1, lambda_sogob);
  uint64_t event_paric = event_mgr->RegisterKeyListener(1, lambda_paric);
  ASSERT_NE(event_sogob, event_paric);
  event_mgr->ProcessKeyEvent(NULL, 1, 0, 0, 0);
  ASSERT_EQ(1, sogob);
  ASSERT_EQ(1, paric);

  sogob = 0;
  paric = 0;
  // remove event and see what hapen
  event_mgr->RemoveKeyListener(event_sogob);
  event_mgr->ProcessKeyEvent(NULL, 1, 0, 0, 0);
  ASSERT_EQ(0, sogob);
  ASSERT_EQ(1, paric);
}

TEST_F(EventManagerTests, CreateMultipleEventsOnDifferentKeys) {
  int dogecoin = 0;
  int fortnite = 0;
  int feet_pic = 0;

  auto l_d = [&dogecoin](int, int, int) {
    dogecoin++;
  };

  auto l_fo = [&fortnite](int, int, int) {
    fortnite++;
  };

  auto l_fe = [&feet_pic](int, int, int) {
    feet_pic++;
  };

  uint64_t event_d = event_mgr->RegisterKeyListener(1, l_d);
  uint64_t event_fo = event_mgr->RegisterKeyListener(2, l_fo);
  uint64_t event_fe = event_mgr->RegisterKeyListener(3, l_fe);

  ASSERT_NE(event_d, event_fo);
  ASSERT_NE(event_fo, event_fe);
  ASSERT_NE(event_d, event_fe);

  event_mgr->ProcessKeyEvent(NULL, 1, 0, 0, 0);

  ASSERT_EQ(0, fortnite);
  ASSERT_EQ(0, feet_pic);
  ASSERT_EQ(1, dogecoin);

  event_mgr->ProcessKeyEvent(NULL, 2, 0, 0, 0);

  ASSERT_EQ(1, fortnite);
  ASSERT_EQ(0, feet_pic);
  ASSERT_EQ(1, dogecoin);

  event_mgr->ProcessKeyEvent(NULL, 3, 0, 0, 0);

  ASSERT_EQ(1, fortnite);
  ASSERT_EQ(1, feet_pic);
  ASSERT_EQ(1, dogecoin);

  event_mgr->RemoveKeyListener(event_d);
  event_mgr->ProcessKeyEvent(NULL, 1, 0, 0, 0);
  
  ASSERT_EQ(1, fortnite);
  ASSERT_EQ(1, feet_pic);
  ASSERT_EQ(1, dogecoin);
}

