#pragma once
#include <vector>
#include <memory>

namespace ngf {

class Graph;
struct GraphEdge;

class AStarAlgorithm {
public:
  AStarAlgorithm(const Graph &graph, int source, int target);

  [[nodiscard]] std::vector<int> getPath() const;

private:
  void search();

private:
  const Graph &m_graph;
  std::vector<std::shared_ptr<GraphEdge>> m_SPT; // The Shortest Path Tree
  std::vector<float> m_gCost; //This array will store the G cost of each node
  std::vector<float> m_fCost; //This array will store the F cost of each node
  std::vector<std::shared_ptr<GraphEdge>> m_SF; // The Search Frontier
  int m_source;
  int m_target;
};
}

