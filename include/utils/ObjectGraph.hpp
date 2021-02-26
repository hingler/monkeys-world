#ifndef OBJECT_GRAPH_H_
#define OBJECT_GRAPH_H_

#include <cinttypes>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace monkeysworld {
namespace utils {

/**
 *  An ObjectGraph is a directed, unweighted graph which 
 *  represents objects in our component hierarchy as nodes,
 *  and the connections between them as edges.
 */ 
class ObjectGraph {

 public:
  /**
   *  Creates a new ObjectGraph.
   */ 
  ObjectGraph();

  /**
   *  Adds a new node to the graph.
   *  Does nothing if the node is already present in the graph.
   *  @param node - the ID of the new node.
   */ 
  void AddNode(uint64_t node);

  /**
   *  Adds a new edge to the graph.
   *  If `parent` or `child` cannot be found on the graph, this function creates them.
   *  If the edge already exists, this function does nothing.
   *  @param parent - the node from which this edge originates.
   *  @param child - the destination of this edge.
   */ 
  void AddEdge(uint64_t parent, uint64_t child);

  /**
   *  @param node - the node we are looking for.
   *  @returns true if the node could be found, false otherwise.
   */ 
  bool ContainsNode(uint64_t node);

  /**
   *  @returns the number of nodes in the graph.
   */ 
  std::size_t GetNodeCount();

  /**
   *  @returns the number of edges in the graph.
   */ 
  std::size_t GetEdgeCount();

  /**
   *  Performs a topological sort on the contents of this graph.
   *  If a topological sort cannot be found, returns an empty vector.
   */ 
  std::vector<uint64_t> TopoSort();
 private:
  // stores nodes and their children
  std::unordered_map<uint64_t, std::unordered_set<uint64_t>> nodes_;
};

}
}

#endif