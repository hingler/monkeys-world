#include <utils/IDGenerator.hpp>

#include <gtest/gtest.h>

using ::monkeysworld::utils::IDGenerator;

TEST(IDGeneratorTests, GetSingleID) {
  IDGenerator gen;
  uint64_t id = gen.GetUniqueId();
  ASSERT_EQ(1, id);
}