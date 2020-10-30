#include <ngf/Math/PathFinding/Graph.h>
#include <ngf/Math/PathFinding/GraphEdge.h>

namespace ngf {
std::shared_ptr<GraphEdge> Graph::getEdge(int from, int to) {
  for (auto &e : edges[from]) {
    if (e->to == to) {
      return e;
    }
  }
  return nullptr;
}

int Graph::addNode(glm::vec2 node) {
  nodes.push_back(node);
  edges.emplace_back();
  return 0;
}

void Graph::addEdge(const std::shared_ptr<GraphEdge> &edge) {
  if (getEdge(edge->from, edge->to) == nullptr) {
    edges[edge->from].push_back(edge);
  }
  if (getEdge(edge->to, edge->from) == nullptr) {
    auto e = std::make_shared<GraphEdge>(edge->to, edge->from, edge->cost);
    edges[edge->to].push_back(e);
  }
}
} // namespace ngf
