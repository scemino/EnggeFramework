#include <ngf/Math/PathFinding/Graph.h>
#include <ngf/Math/PathFinding/GraphEdge.h>
#include "AStarAlgorithm.h"
#include "IndexedPriorityQueue.h"

namespace ngf {

AStarAlgorithm::AStarAlgorithm(const Graph &graph, int source, int target)
    : m_graph(graph), m_source(source), m_target(target) {
  m_gCost.resize(m_graph.nodes.size(), 0);
  m_fCost.resize(m_graph.nodes.size(), 0);
  m_SPT.resize(m_graph.nodes.size(), nullptr);
  m_SF.resize(m_graph.nodes.size(), nullptr);
  search();
}

std::vector<int> AStarAlgorithm::getPath() const {
  if (m_target < 0)
    return {};
  int nd = m_target;
  std::vector<int> path;
  path.push_back(nd);
  while ((nd != m_source) && (m_SPT[nd] != nullptr)) {
    nd = m_SPT[nd]->from;
    path.push_back(nd);
  }
  std::reverse(path.begin(), path.end());
  return path;
}

void AStarAlgorithm::search() {
  IndexedPriorityQueue pq(m_fCost);
  pq.insert(m_source);
  while (!pq.isEmpty()) {
    int NCN = pq.pop();
    m_SPT[NCN] = m_SF[NCN];
    if (NCN == m_target)
      return;
    auto &edges = m_graph.edges[NCN];
    for (auto &edge : edges) {
      float Hcost = length(m_graph.nodes[edge->to] - m_graph.nodes[m_target]);
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
