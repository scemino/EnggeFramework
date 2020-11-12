#include <ngf/Graphics/CircleShape.h>

namespace ngf {
CircleShape::CircleShape(float radius, std::size_t pointCount)
    : m_radius(radius), m_pointCount(pointCount) {
  updateGeometry();
  updateBounds(ngf::frect::fromPositionSize({0.0f, 0.0f}, {2 * m_radius, 2 * m_radius}));
}

void CircleShape::setRadius(float radius) {
  if (m_radius == radius)
    return;

  m_radius = radius;
  updateGeometry();
  updateBounds(ngf::frect::fromPositionSize({0.0f, 0.0f}, {2 * m_radius, 2 * m_radius}));
}

void CircleShape::setPointCount(std::size_t pointCount) {
  if (m_pointCount == pointCount) {
    return;
  }

  m_pointCount = pointCount;
  updateGeometry();
}

std::size_t CircleShape::getPointCount() const {
  return m_pointCount;
}

glm::vec2 CircleShape::getPoint(std::size_t index) const {
  assert(index < m_pointCount);
  float angle = static_cast<float>(index) * 2.f * M_PI / m_pointCount - M_PI_2;
  float x = m_radius * std::cos(angle);
  float y = m_radius * std::sin(angle);
  return {m_radius + x, m_radius + y};
}
}
