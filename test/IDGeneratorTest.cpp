#include <utils/IDGenerator.hpp>

#include <gtest/gtest.h>
#include <set>

using ::monkeysworld::utils::IDGenerator;

TEST(IDGeneratorTests, GetSingleID) {
  IDGenerator gen;
  uint64_t id = gen.GetUniqueId();
  ASSERT_EQ(2, id);
}

TEST(IDGeneratorTests, GenerateMultipleIDs) {
  IDGenerator gen;
  for (int i = 0; i < 128; i++) {
    gen.GetUniqueId();
  }

  ASSERT_EQ(130, gen.GetUniqueId());
}

void WasteIDs(IDGenerator& gen) {
  for (int i = 0; i < 16; i++) {
    gen.GetUniqueId();
  }
}

TEST(IDGeneratorTests, MultiThreadGeneration) {
  IDGenerator gen;
  std::thread threads[8];
  for (int i = 0; i < 8; i++) {
    // func, args list
    threads[i] = std::thread(WasteIDs, std::ref(gen));
    // other way: std::thread([&]{WasteIDs(gen)})
  }

  for (int i = 0; i < 8; i++) {
    threads[i].join();
  }

  ASSERT_EQ(130, gen.GetUniqueId());

}

TEST(IDGeneratorTests, AvoidAddedID) {
  IDGenerator gen;
  gen.RegisterUniqueId(2);
  gen.RegisterUniqueId(3);
  gen.RegisterUniqueId(27);
  ASSERT_EQ(4, gen.GetUniqueId());
  for (int i = 0; i < 32; i++) {
    gen.GetUniqueId();
  }

  // 33 gens with 3 skips -- the next thing we see should be 
  ASSERT_EQ(38, gen.GetUniqueId());
}

TEST(IDGeneratorTests, EnsureIDsUnique) {
  IDGenerator gen;
  std::set<uint64_t> ids;
  for (int i = 0; i < 32; i++) {
    uint64_t id = gen.GetUniqueId();
    ASSERT_TRUE(ids.find(id) == ids.end());
    ids.insert(id);
  }
}