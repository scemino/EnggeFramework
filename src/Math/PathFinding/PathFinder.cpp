#include <algorithm>
#include <cmath>
#include "AStarAlgorithm.h"
#include <ngf/Math/PathFinding/Graph.h>
#include <ngf/Math/PathFinding/GraphEdge.h>
#include <ngf/Math/PathFinding/PathFinder.h>
#include <ngf/Math/PathFinding/Walkbox.h>
#include <ngf/Math/Segment.h>

namespace ngf {

namespace {
inline constexpr float Epsilon = 1E-9;

inline bool betw(float l, float r, float x) {
  return fmin(l, r) <= x + Epsilon && x <= fmax(l, r) + Epsilon;
}

constexpr float det(float a, float b, float c, float d) {
  return a * d - b * c;
}

constexpr bool intersect_1d(float a, float b, float c, float d) {
  if (a > b)
    std::swap(a, b);
  if (c > d)
    std::swap(c, d);
  return fmax(a, c) <= fmin(b, d) + Epsilon;
}

constexpr bool less(const glm::vec2 &p1, const glm::vec2 &p2) {
  return p1.x < p2.x - Epsilon || (fabs(p1.x - p2.x) < Epsilon && p1.y < p2.y - Epsilon);
}

constexpr bool intersect(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 d) {
  if (!intersect_1d(a.x, b.x, c.x, d.x) || !intersect_1d(a.y, b.y, c.y, d.y))
    return false;
  Segment m(a, b);
  Segment n(c, d);
  double zn = det(m.a, m.b, n.a, n.b);
  if (abs(zn) < Epsilon) {
    if (abs(m.distance(c)) > Epsilon || abs(n.distance(a)) > Epsilon)
      return false;
    if (less(b, a))
      std::swap(a, b);
    if (less(d, c))
      std::swap(c, d);
    return true;
  } else {
    auto lx = -det(m.c, m.b, n.c, n.b) / zn;
    auto ly = -det(m.a, m.c, n.a, n.c) / zn;
    return betw(a.x, b.x, lx) && betw(a.y, b.y, ly) &&
        betw(c.x, d.x, lx) && betw(c.y, d.y, ly);
  }
}

constexpr bool lineSegmentsCross(const glm::vec2 &a,
                                 const glm::vec2 &b,
                                 const glm::vec2 &c,
                                 const glm::vec2 &d) {
  return intersect(a, b, c, d);
}

}

std::shared_ptr<Graph> PathFinder::createGraph() {
  auto mainWalkgraph = std::make_shared<Graph>();
  for (const auto &walkbox : m_walkboxes) {
    if (walkbox.size() <= 2)
      continue;

    auto isEnabled = walkbox.isEnabled();
    for (size_t i = 0; i < walkbox.size(); i++) {
      if (walkbox.isVertexConcave(i) != isEnabled)
        continue;

      const auto &vertex = walkbox.at(i);
      mainWalkgraph->concaveVertices.push_back(vertex);
      mainWalkgraph->addNode((glm::vec2) vertex);
    }
  }
  for (size_t i = 0; i < mainWalkgraph->concaveVertices.size(); i++) {
    for (size_t j = 0; j < mainWalkgraph->concaveVertices.size(); j++) {
      auto c1 = (glm::vec2) mainWalkgraph->concaveVertices[i];
      auto c2 = (glm::vec2) mainWalkgraph->concaveVertices[j];
      if (inLineOfSight(c1, c2)) {
        mainWalkgraph->addEdge(std::make_shared<GraphEdge>(i, j, distance(c1, c2)));
      }
    }
  }
  return mainWalkgraph;
}

std::vector<glm::vec2> PathFinder::calculatePath(glm::vec2 from, glm::vec2 to) {
  if (m_walkboxes.empty())
    return {};

  if (!m_graph) {
    m_graph = createGraph();
  }

  Graph walkgraph(*m_graph);
  //create new node on start position
  auto startNodeIndex = static_cast<int>(walkgraph.nodes.size());
  if (!m_walkboxes[0].inside(from)) {
    from = m_walkboxes[0].getClosestPointOnEdge(from);
  }
  if (!m_walkboxes[0].inside(to)) {
    to = m_walkboxes[0].getClosestPointOnEdge(to);
  }

  //Are there more polygons? Then check if endpoint is inside oine of them and find closest point on edge
  if (m_walkboxes.size() > 1) {
    for (int i = 1; i < static_cast<int>(m_walkboxes.size()); i++) {
      if (m_walkboxes[i].inside(to)) {
        to = m_walkboxes[i].getClosestPointOnEdge(to);
        break;
      }
    }
  }

  walkgraph.addNode(from);

  for (int i = 0; i < static_cast<int>(walkgraph.concaveVertices.size()); i++) {
    auto c = (glm::vec2) walkgraph.concaveVertices[i];
    if (inLineOfSight(from, c)) {
      walkgraph.addEdge(std::make_shared<GraphEdge>(startNodeIndex, i, distance(from, c)));
    }
  }

  //create new node on end position
  int endNodeIndex = static_cast<int>(walkgraph.nodes.size());
  walkgraph.addNode(to);

  for (int i = 0; i < static_cast<int>(walkgraph.concaveVertices.size()); i++) {
    auto c = (glm::vec2) walkgraph.concaveVertices[i];
    if (inLineOfSight(to, c)) {
      auto edge = std::make_shared<GraphEdge>(i, endNodeIndex, distance(to, c));
      walkgraph.addEdge(edge);
    }
  }
  if (inLineOfSight(from, to)) {
    auto edge = std::make_shared<GraphEdge>(startNodeIndex, endNodeIndex, distance(from, to));
    walkgraph.addEdge(edge);
  }

  AStarAlgorithm astar(walkgraph, startNodeIndex, endNodeIndex);
  auto indices = astar.getPath();
  std::vector<glm::vec2> path;
  path.reserve(walkgraph.nodes.size());
  std::transform(indices.begin(), indices.end(), std::back_inserter(path), [&walkgraph](auto i) {
    return walkgraph.nodes[i];
  });
  return path;
}

bool PathFinder::inLineOfSight(glm::vec2 start, glm::vec2 end) {
  const float epsilon = 0.5f;

  // Not in LOS if any of the ends is outside the polygon
  if (!m_walkboxes[0].inside(start) || !m_walkboxes[0].inside(end)) {
    return false;
  }

  // In LOS if it's the same start and end location
  if (length(start - end) < epsilon) {
    return true;
  }

  // Not in LOS if any edge is intersected by the start-end line segment
  for (const auto &walkbox : m_walkboxes) {
    auto size = walkbox.size();
    for (size_t i = 0; i < size; i++) {
      auto v1 = (glm::vec2) walkbox.at(i);
      auto v2 = (glm::vec2) walkbox.at((i + 1) % size);
      if (!lineSegmentsCross(start, end, v1, v2))
        continue;

      //In some cases a 'snapped' endpoint is just a little over the line due to rounding errors. So a 0.5 margin is used to tackle those cases.
      if (Walkbox::distanceToSegment(start, v1, v2) > epsilon
          && Walkbox::distanceToSegment(end, v1, v2) > epsilon) {
        return false;
      }
    }
  }

  // Finally the middle point in the segment determines if in LOS or not
  glm::vec2 v2 = (start + end) / 2.f;
  auto inside = m_walkboxes[0].inside(v2);
  for (int i = 1; i < static_cast<int>(m_walkboxes.size()); i++) {
    if (m_walkboxes[i].inside(v2, false)) {
      inside = false;
    }
  }
  return inside;
}
} // namespace ngf