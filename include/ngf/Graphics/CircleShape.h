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
  explicit CircleShape(float radius = 0, std::size_t pointCount = 30);

  /// @brief sets the radius of the circle.
  /// \param radius Radius to set to the circle.
  void setRadius(float radius);

  /// @brief Sets the number of points composing the circle.
  /// \param pointCount Number of points composing the circle.
  void setPointCount(std::size_t pointCount);

  /// @brief Gets the number of points composing the circle.
  /// \return the number of points composing the circle.
  std::size_t getPointCount() const override;

  /// @brief Gets a point composing the circle at the specified index.
  /// \param index Index of the point to get.
  /// \return Point composing the circle at the specified index.
  glm::vec2 getPoint(std::size_t index) const override;

private:
  float m_radius;
  std::size_t m_pointCount;
};
}