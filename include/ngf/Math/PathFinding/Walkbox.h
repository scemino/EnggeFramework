#pragma once
#include <vector>
#include <list>
#include <iostream>
#include <glm/vec2.hpp>

namespace ngf {

enum class YAxisDirection {
  Up,
  Down
};

/// @ brief A walkbox is a polygon where an actor can walk.
class Walkbox {
public:
  using iterator = std::vector<glm::ivec2>::iterator;
  using const_iterator = std::vector<glm::ivec2>::const_iterator;
  using size_type = std::vector<glm::ivec2>::size_type;
  using reference = std::vector<glm::ivec2>::reference;
  using const_reference = std::vector<glm::ivec2>::const_reference;

  /// @brief Creates a walkbox from a container of points.
  /// \tparam TContainer Type of the container (std::vector, std::array, etc.).
  /// \param points Container of points.
  template<typename TContainer>
  explicit Walkbox(const TContainer &points);
  /// @brief Creates a walkbox from iterators of points.
  /// \tparam TIterator Type of the iterators.
  /// \param begin Begin iterator of points.
  /// \param end End iterator of points.
  template<typename TIterator>
  Walkbox(TIterator begin, TIterator end);

  /// @brief Sets the y-direction of the points.
  /// \param direction true, if the points have a y-axis oriented to the top of the screen, otherwise false.
  void setYAxisDirection(YAxisDirection direction) { m_yAxisDirection = direction; }
  /// @brief Gets the y-direction of the points.
  /// \return true, if the points have a y-axis oriented to the top of the screen, otherwise false.
  [[nodiscard]] YAxisDirection getYAxisDirection() const { return m_yAxisDirection; }

  /// @brief Sets the name of the walkbox.
  /// \param name The name of the walkbox.
  void setName(const std::string &name) { m_name = name; }
  /// @brief Gets the name of the walkbox.
  /// \return The name of the walkbox.
  [[nodiscard]] const std::string &getName() const { return m_name; }

  /// @brief Sets whether or not the walkbox is enabled, meaning if an actor is allowed to walk in this walkbox.
  /// \param isEnabled true if the walkbox is enabled, otherwise false.
  void setEnabled(bool isEnabled) { m_isEnabled = isEnabled; }
  /// @brief Indicates whether or not the walkbox is enabled, meaning if an actor is allowed to walk in this walkbox.
  /// \return true if the walkbox is enabled, otherwise false.
  [[nodiscard]] bool isEnabled() const { return m_isEnabled; }

  /// @brief Sets whether or not the walkbox is visible.
  /// \param visible true if the walkbox is visible, otherwise false.
  void setVisible(bool visible) { m_visible = visible; }
  /// @brief Indicates whether or not the walkbox is visible.
  /// \return true if the walkbox is visible, otherwise false.
  [[nodiscard]] bool isVisible() const { return m_visible; }

  /// Gets the begin iterator of the points of the walkbox.
  /// \return the begin iterator of the points of the walkbox.
  iterator begin() { return m_polygon.begin(); }
  /// Gets the end iterator of the points of the walkbox.
  /// \return the end iterator of the points of the walkbox.
  iterator end() { return m_polygon.end(); }

  /// Gets the constant begin iterator of the points of the walkbox.
  /// \return the constant begin iterator of the points of the walkbox.
  [[nodiscard]] const_iterator cbegin() const { return m_polygon.cbegin(); }
  /// Gets the constant end iterator of the points of the walkbox.
  /// \return the constant end iterator of the points of the walkbox.
  [[nodiscard]] const_iterator cend() const { return m_polygon.cend(); }

  /// Gets the number of points in this walkbox.
  /// \return The number of points in this walkbox.
  [[nodiscard]] size_type size() const { return m_polygon.size(); }
  /// Gets the point at the specified index.
  /// \param index Index of the point to retrieve.
  /// \return A constant reference to the point.
  [[nodiscard]] const_reference at(size_type index) const { return m_polygon.at(index); }

  /// @brief Indicates whether or not the specified position is inside this walkbox.
  /// \param position Position to test.
  /// \param toleranceOnOutside true if we want to tolerate point which are almost outside of the walkbox.
  /// \return true if the point is inside the walkbox, otherwise false.
  [[nodiscard]] bool inside(const glm::vec2 &position, bool toleranceOnOutside = true) const;

  /// @brief Indicates whether or not the vertex at the given index is convex or not.
  /// \param index Index of the point to test.
  /// \return true if the vertex at the given index is convex, otherwise false.
  [[nodiscard]] bool isVertexConcave(size_type index) const;

  static float distanceToSegment(glm::vec2 p, glm::vec2 v, glm::vec2 w);

  /// @brief Gets the closest point on the edge of the walkbox from the given point.
  /// \param point Point from where to find the closest point on the edge of the walkbox.
  /// \return The closest point on the edge of the walkbox.
  [[nodiscard]] glm::vec2 getClosestPointOnEdge(glm::vec2 point) const;

  friend std::ostream &operator<<(std::ostream &os, const Walkbox &walkbox);

  /// @brief Inserts a point to the walkbox at a specified position.
  /// \param pos Position where to insert the new point of the polygon.
  /// \param point Point to insert in the walkbox.
  void insert(const_iterator pos, const_reference point) { m_polygon.insert(pos, point); }

private:
  bool m_visible{true};
  std::vector<glm::ivec2> m_polygon;
  std::string m_name;
  YAxisDirection m_yAxisDirection{YAxisDirection::Up};
  bool m_isEnabled{true};
};

template<typename TContainer>
Walkbox::Walkbox(const TContainer &points)
    : Walkbox(points.begin(), points.end()) {
}

template<typename TIterator>
Walkbox::Walkbox(TIterator begin, TIterator end)
    : m_polygon(begin, end) {
}

} // namespace ngf
