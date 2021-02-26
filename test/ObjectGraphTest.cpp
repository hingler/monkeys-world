#include <gtest/gtest.h>

#include <boost/log/trivial.hpp>

#include <utils/ObjectGraph.hpp>

using ::monkeysworld::utils::ObjectGraph;

TEST(ObjectGraphTests, CreateObjectGraph) {
  ObjectGraph o;
  o.AddNode(1);
  o.AddNode(2);
  o.AddEdge(1, 2);

  ASSERT_EQ(1, o.GetEdgeCount());
  ASSERT_EQ(2, o.GetNodeCount());
  ASSERT_TRUE(o.ContainsNode(1));
  ASSERT_FALSE(o.ContainsNode(5));
}

TEST(ObjectGraphTests, AddNonexistentEdge) {
  ObjectGraph o;
  o.AddEdge(1, 2);

  ASSERT_EQ(o.GetEdgeCount(), 1);
  ASSERT_EQ(o.GetNodeCount(), 2);
  ASSERT_TRUE(o.ContainsNode(1));
  ASSERT_FALSE(o.ContainsNode(5));
}

TEST(ObjectGraphTests, TopoSort) {
  ObjectGraph o;
  o.AddEdge(1, 2);
  o.AddEdge(2, 3);
  std::vector<uint64_t> res = o.TopoSort();
  ASSERT_EQ(3, res.size());
  ASSERT_EQ(1, res[0]);
  ASSERT_EQ(2, res[1]);
  ASSERT_EQ(3, res[2]);
}

TEST(ObjectGraphTests, TopoSort2) {
  ObjectGraph o;
  o.AddEdge(1, 3);
  o.AddEdge(2, 3);
  o.AddEdge(2, 4);
  o.AddEdge(3, 5);
  o.AddEdge(4, 5);
  o.AddEdge(3, 4);
  std::vector<uint64_t> res = o.TopoSort();
  ASSERT_EQ(5, res.size());
  ASSERT_EQ(1, res[0]);
  ASSERT_EQ(2, res[1]);
  ASSERT_EQ(3, res[2]);
  ASSERT_EQ(4, res[3]);
  ASSERT_EQ(5, res[4]);
}
