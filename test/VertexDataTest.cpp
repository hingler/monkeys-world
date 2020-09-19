#include "storage/VertexData.hpp"
#include "storage/VertexPacketTypes.hpp"
#include "opengl/VertexDataContext.hpp"
#include "opengl/VertexDataContextGL.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <memory>

// TODO: add tests for vertex data
//  - verify contents
//  - verify packet strides ig
//      - go in as void and extract specific expected params
//  - verify accurate size, etc


namespace screenspacemanagertest {
using ::screenspacemanager::storage::VertexData;
using ::screenspacemanager::opengl::VertexDataContext;

struct DummyPacket {
  int data;
  static void Bind() {

  };
};

/**
 *  Mock for vertex data context
 */ 
template <typename Packet>
class MockVertexContext : public VertexDataContext<Packet> {
 public:
  MOCK_METHOD(void, PopulateBuffersAndBind, (const std::vector<Packet>& data, const std::vector<int>& indices), (override));
};


class VertexDataTests : public ::testing::Test {
 protected:
  void SetUp() override {
    std::cout << "running vertex data tests..." << std::endl;
  }
};

// gl is not initialized!



TEST_F(VertexDataTests, CreateDummyPacket) {
	std::cout << "testing123" << std::endl;
  VertexData<DummyPacket> data(std::make_unique<MockVertexContext<DummyPacket>>());
  data.AddVertex({1});
  data.AddVertex({2});
  data.AddVertex({3});

  const int* contents = reinterpret_cast<const int*>(data.GetVertexData());
  ASSERT_EQ(1, contents[0]);
  ASSERT_EQ(2, contents[1]);
  ASSERT_EQ(3, contents[2]);

  data.PointToVertexAttribs();
}
};