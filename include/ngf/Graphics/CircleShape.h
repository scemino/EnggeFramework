#pragma once
#include <cmath>
#include <ngf/Graphics/Rect.h>
#include <ngf/Graphics/Shape.h>

namespace ngf {

/// @brief Specialized shape representing a circle.
class CircleShape : public Shape {
public:
  /// @brief Creates a circle shape with a specified radius and a number of points composing the circle.
  /// \param radius Radius of the circle.
  /// \param pointCount Number of points composing the circle.
  explicit CircleShape(float radius = 0, std::size_t pointCount = 30)
      : m_radius(radius), m_pointCount(pointCount) {
    updateGeometry();
    updateBounds(ngf::frect::fromPositionSize({0.0f, 0.0f}, {2 * m_radius, 2 * m_radius}));
  }

  /// @brief sets the radius of the circle.
  /// \param radius Radius to set to the circle.
  void setRadius(float radius) {
    if (m_radius == radius)
      return;

    m_radius = radius;
    updateGeometry();
    updateBounds(ngf::frect::fromPositionSize({0.0f, 0.0f}, {2 * m_radius, 2 * m_radius}));
  }

  /// @brief Sets the number of points composing the circle.
  /// \param pointCount Number of points composing the circle.
  void setPointCount(std::size_t pointCount) {
    if (m_pointCount == pointCount) {
      return;
    }

    m_pointCount = pointCount;
    updateGeometry();
  }

  /// @brief Gets the number of points composing the circle.
  /// \return the number of points composing the circle.
  std::size_t getPointCount() const override {
    return m_pointCount;
  }

  /// @brief Gets a point composing the circle at the specified index.
  /// \param index Index of the point to get.
  /// \return Point composing the circle at the specified index.
  glm::vec2 getPoint(std::size_t index) const override {
    assert(index < m_pointCount);
    float angle = static_cast<float>(index) * 2.f * M_PI / m_pointCount - M_PI_2;
    float x = m_radius * std::cos(angle);
    float y = m_radius * std::sin(angle);
    return {m_radius + x, m_radius + y};
  }

private:
  float m_radius;
  std::size_t m_pointCount;
};
}