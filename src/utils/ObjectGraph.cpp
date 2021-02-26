#include <utils/ObjectGraph.hpp>

#include <queue>

#include <boost/log/trivial.hpp>

namespace monkeysworld {
namespace utils {

ObjectGraph::ObjectGraph() {

}

void ObjectGraph::AddNode(uint64_t node) {
  auto p = nodes_.find(node);
  if (p == nodes_.end()) {
    // im a bit worried about managing all these sets -- but i guess its what we need
    nodes_.insert(std::make_pair(node, std::unordered_set<uint64_t>()));
  }
}

void ObjectGraph::AddEdge(uint64_t parent, uint64_t child) {
  if (!ContainsNode(parent)) {
    AddNode(parent);
  }

  if (!ContainsNode(child)) {
    AddNode(child);
  }

  nodes_.at(parent).insert(child);
}

bool ObjectGraph::ContainsNode(uint64_t node) {
  auto p = nodes_.find(node);
  return (p != nodes_.end());
}

std::size_t ObjectGraph::GetNodeCount() {
  return nodes_.size();
}

std::size_t ObjectGraph::GetEdgeCount() {
  std::size_t res = 0;
  for (auto p : nodes_) {
    res += p.second.size();
  }

  return res;
}

std::vector<uint64_t> ObjectGraph::TopoSort() {
  std::unordered_map<uint64_t, std::size_t> in_deg;
  std::queue<uint64_t> visit_queue;
  std::vector<uint64_t> res;

  for (auto p : nodes_) {
    in_deg.insert(std::make_pair(p.first, 0));
  }

  for (auto p : nodes_) {
    for (auto n : p.second) {
      in_deg.at(n)++;
    }
  }
  auto itr = in_deg.begin();
  while (itr != in_deg.end()) {
    if (itr->second == 0) {
      visit_queue.push(itr->first);
      itr = in_deg.erase(itr);
    } else {
      itr++;
    }
  }

  while (!visit_queue.empty()) {
    // grab a node from the visit queue, add it to res.
    uint64_t node = visit_queue.front();
    visit_queue.pop();
    res.push_back(node);
    // guaranteed to exist
    // for each of its children in nodes:
    for (uint64_t node : nodes_.find(node)->second) {
      size_t& np = in_deg.at(node);
    // decrement that child in in_deg, if it is present.
      np--;
    // if its visit count is 0, remove it from in_deg, and add it to visit_queue.
      if (np <= 0) {
        in_deg.erase(node);
        visit_queue.push(node);
      }
    }
  }

  // a valid topo sort exists if the size of res is the same as nodes_.
  if (res.size() == nodes_.size()) {
    return res;
  }

  return std::vector<uint64_t>();
}

}
}