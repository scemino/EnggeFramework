#include <cmath>
#include <algorithm>
#include <ngf/Math/PathFinding/Walkbox.h>
#include <ngf/Math/Math.h>

namespace ngf {

namespace {
constexpr float distanceSquared(const glm::vec2 &vector1, const glm::vec2 &vector2) {
  float dx = vector1.x - vector2.x;
  float dy = vector1.y - vector2.y;

  return dx * dx + dy * dy;
}

constexpr float distanceToSegmentSquared(const glm::vec2 &p, const glm::vec2 &v, const glm::vec2 &w) {
  float l2 = distanceSquared(v, w);
  if (l2 == 0)
    return distanceSquared(p, v);
  float t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
  if (t < 0)
    return distanceSquared(p, v);
  if (t > 1)
    return distanceSquared(p, w);
  return distanceSquared(p, glm::vec2(v.x + t * (w.x - v.x), v.y + t * (w.y - v.y)));
}
}

float Walkbox::distanceToSegment(glm::vec2 p, glm::vec2 v, glm::vec2 w) {
  return sqrt(distanceToSegmentSquared(p, v, w));
}

glm::vec2 Walkbox::getClosestPointOnEdge(glm::vec2 p3) const {
  int vi1 = -1;
  int vi2 = -1;
  float minDist = 100000;

  for (int i = 0; i < static_cast<int>(m_polygon.size()); i++) {
    auto dist = distanceToSegment(p3, (glm::vec2) m_polygon[i], (glm::vec2) m_polygon[(i + 1) % m_polygon.size()]);
    if (dist < minDist) {
      minDist = dist;
      vi1 = i;
      vi2 = (i + 1) % static_cast<int>(m_polygon.size());
    }
  }
  glm::ivec2 p1 = m_polygon[vi1];
  glm::ivec2 p2 = m_polygon[vi2];

  float x1 = p1.x;
  float y1 = p1.y;
  float x2 = p2.x;
  float y2 = p2.y;
  float x3 = p3.x;
  float y3 = p3.y;

  float u = (((x3 - x1) * (x2 - x1)) + ((y3 - y1) * (y2 - y1))) / (((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));

  float xu = x1 + u * (x2 - x1);
  float yu = y1 + u * (y2 - y1);

  if (u < 0)
    return glm::vec2(x1, y1);
  if (u > 1)
    return glm::vec2(x2, y2);
  return glm::vec2(xu, yu);
}

bool Walkbox::isVertexConcave(size_type vertex) const {
  auto current = m_polygon[vertex];
  auto next = m_polygon[(vertex + 1) % m_polygon.size()];
  auto previous = m_polygon[vertex == 0 ? m_polygon.size() - 1 : vertex - 1];

  auto left = glm::vec2(current.x - previous.x, current.y - previous.y);
  auto right = glm::vec2(next.x - current.x, next.y - current.y);

  auto cross = (left.x * right.y) - (left.y * right.x);
  return m_yAxisDirection == YAxisDirection::Up == cross >= 0;
}

bool Walkbox::inside(const glm::vec2 &position, bool toleranceOnOutside) const {
  glm::vec2 point = position;
  const float epsilon = 1.0f;
  bool inside = false;

  // Must have 3 or more edges
  if (m_polygon.size() < 3)
    return false;

  auto oldPoint = (glm::vec2) m_polygon[m_polygon.size() - 1];
  float oldSqDist = distanceSquared(oldPoint, point);

  for (auto nPoint : m_polygon) {
    auto newPoint = (glm::vec2) nPoint;
    float newSqDist = distanceSquared(newPoint, point);

    if (oldSqDist + newSqDist + 2.0f * sqrt(oldSqDist * newSqDist) - distanceSquared(newPoint, oldPoint) < epsilon)
      return toleranceOnOutside;

    glm::vec2 left;
    glm::vec2 right;
    if (newPoint.x > oldPoint.x) {
      left = oldPoint;
      right = newPoint;
    } else {
      left = newPoint;
      right = oldPoint;
    }

    if (left.x < point.x && point.x <= right.x
        && (point.y - left.y) * (right.x - left.x) < (right.y - left.y) * (point.x - left.x))
      inside = !inside;

    oldPoint = newPoint;
    oldSqDist = newSqDist;
  }
  return inside;
}

std::ostream &operator<<(std::ostream &os, const Walkbox &walkbox) {
  os << "[ ";
  for (auto vertex : walkbox.m_polygon) {
    os << '(' << vertex.x << ',' << vertex.y << ") ";
  }
  os << ']';
  return os;
}

} // namespace ng
