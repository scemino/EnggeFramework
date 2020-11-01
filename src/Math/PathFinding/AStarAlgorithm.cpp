#include <ngf/Math/PathFinding/Graph.h>
#include <ngf/Math/PathFinding/GraphEdge.h>
#include <algorithm>
#include "AStarAlgorithm.h"
#include "IndexedPriorityQueue.h"

namespace ngf {

AStarAlgorithm::AStarAlgorithm(const Graph &graph)
    : m_graph(graph) {
  m_gCost.resize(m_graph.nodes.size(), 0);
  m_fCost.resize(m_graph.nodes.size(), 0);
  m_SPT.resize(m_graph.nodes.size(), nullptr);
  m_SF.resize(m_graph.nodes.size(), nullptr);
}

std::vector<int> AStarAlgorithm::getPath(const Graph &graph, int source, int target) {
  AStarAlgorithm astar(graph);
  if (target < 0)
    return {};
  astar.search(source, target);

  int nd = target;
  std::vector<int> path;
  path.push_back(nd);
  while ((nd != source) && (astar.m_SPT[nd] != nullptr)) {
    nd = astar.m_SPT[nd]->from;
    path.push_back(nd);
  }
  std::reverse(path.begin(), path.end());
  return path;
}

void AStarAlgorithm::search(int source, int target) {
  IndexedPriorityQueue pq(m_fCost);
  pq.insert(source);
  while (!pq.isEmpty()) {
    int NCN = pq.pop();
    m_SPT[NCN] = m_SF[NCN];
    if (NCN == target)
      return;
    auto &edges = m_graph.edges[NCN];
    for (auto &edge : edges) {
      float Hcost = length(m_graph.nodes[edge->to] - m_graph.nodes[target]);
      float Gcost = m_gCost[NCN] + edge->cost;
      if (m_SF[edge->to] == nullptr) {
        m_fCost[edge->to] = Gcost + Hcost;
        m_gCost[edge->to] = Gcost;
        pq.insert(edge->to);
        m_SF[edge->to] = edge;
      } else if ((Gcost < m_gCost[edge->to]) && (m_SPT[edge->to] == nullptr)) {
        m_fCost[edge->to] = Gcost + Hcost;
        m_gCost[edge->to] = Gcost;
        pq.reorderUp();
        m_SF[edge->to] = edge;
      }
    }
  }
}
}
