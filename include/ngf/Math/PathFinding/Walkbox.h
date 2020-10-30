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

class Walkbox {
public:
  using iterator = std::vector<glm::ivec2>::iterator;
  using const_iterator = std::vector<glm::ivec2>::const_iterator;
  using size_type = std::vector<glm::ivec2>::size_type;
  using reference = std::vector<glm::ivec2>::reference;
  using const_reference = std::vector<glm::ivec2>::const_reference;

  template<typename TContainer>
  explicit Walkbox(const TContainer &points);
  template<typename TIterator>
  Walkbox(TIterator begin, TIterator end);

  void setYAxisDirection(YAxisDirection direction) { m_yAxisDirection = direction; }
  [[nodiscard]] YAxisDirection getYAxisDirection() const { return m_yAxisDirection; }

  void setName(const std::string &name) { m_name = name; }
  [[nodiscard]] const std::string &getName() const { return m_name; }

  void setEnabled(bool isEnabled) { m_isEnabled = isEnabled; }
  [[nodiscard]] bool isEnabled() const { return m_isEnabled; }

  iterator begin() { return m_polygon.begin(); }
  iterator end() { return m_polygon.end(); }

  [[nodiscard]] const_iterator cbegin() const { return m_polygon.cbegin(); }
  [[nodiscard]] const_iterator cend() const { return m_polygon.cend(); }

  [[nodiscard]] size_type size() const { return m_polygon.size(); }
  [[nodiscard]] const_reference at(size_type index) const { return m_polygon.at(index); }

  [[nodiscard]] bool inside(const glm::vec2 &position, bool toleranceOnOutside = true) const;
  [[nodiscard]] bool isVertexConcave(size_type index) const;
  static float distanceToSegment(glm::vec2 p, glm::vec2 v, glm::vec2 w);
  [[nodiscard]] glm::vec2 getClosestPointOnEdge(glm::vec2 p3) const;

  friend std::ostream &operator<<(std::ostream &os, const Walkbox &walkbox);

private:
  std::vector<glm::ivec2> m_polygon;
  std::string m_name;
  YAxisDirection m_yAxisDirection{YAxisDirection::Up};
  bool m_isEnabled{true};
};

template<typename TContainer>
Walkbox::Walkbox(const TContainer &points)
    : m_polygon(points) {
}

template<typename TIterator>
Walkbox::Walkbox(TIterator begin, TIterator end)
    : m_polygon(begin, end) {
}

} // namespace ngf
