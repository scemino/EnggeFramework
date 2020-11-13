#pragma once
#include <ngf/Graphics/Shape.h>
#include <glm/vec2.hpp>

namespace ngf {
/// @brief Specialized shape representing a convex polygon.
class ConvexShape : public Shape {
public:
  /// @brief Creates a shape from iterators of points.
  /// \tparam TContainer Type of the container (std::vector, std::array, etc.).
  /// \param points Container of points.
  template<typename TContainer>
  explicit ConvexShape(const TContainer &points);

  /// @brief Creates a shape from iterators of points.
  /// \tparam TIterator Type of the iterators.
  /// \param begin Begin iterator of points.
  /// \param end End iterator of points.
  template<typename TIterator>
  ConvexShape(TIterator begin, TIterator end);

  /// @brief Gets the number of points composing the shape.
  /// \return the number of points composing the shape.
  std::size_t getPointCount() const override { return m_points.size(); }

  /// @brief Gets a point composing the shape at the specified index.
  /// \param index Index of the point to get.
  /// \return Point composing the shape at the specified index.
  glm::vec2 getPoint(std::size_t index) const override { return m_points.at(index); }

private:
  std::vector<glm::vec2> m_points;
};

template<typename TContainer>
ConvexShape::ConvexShape(const TContainer &points)
    : ConvexShape(points.begin(), points.end()) {
  updateGeometry();
  updateAutoBounds();
}

template<typename TIterator>
ConvexShape::ConvexShape(TIterator begin, TIterator end)
    : m_points(begin, end) {
  updateGeometry();
  updateAutoBounds();
}
}