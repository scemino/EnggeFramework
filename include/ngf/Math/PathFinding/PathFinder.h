#pragma once
#include <vector>
#include <glm/vec2.hpp>

namespace ngf {
class Walkbox;
class Graph;

/// @brief A PathFinder is used to find a walkable path within one or several walkboxes.
class PathFinder {
public:
  /// @brief Creates a PathFinder from a container of walkboxes.
  /// \tparam TContainer Type of the container of walkboxes.
  /// \param walkboxes Container of walkboxes.
  template<typename TContainer>
  explicit PathFinder(const TContainer &walkboxes);
  /// @brief Creates a PathFinder from begin and end iterators of walkboxes.
  /// \tparam TIterator Type of the iterators.
  /// \param begin Begin iterator.
  /// \param end  End itaerator.
  template<typename TIterator>
  PathFinder(TIterator begin, TIterator end);

  /// @brief Calculates the path between from 'from' point to 'to' point.
  /// \param from Beginning of the path to search.
  /// \param to The destination of the path to find.
  /// \return The path between from 'from' point to 'to' point.
  std::vector<glm::vec2> calculatePath(glm::vec2 from, glm::vec2 to);

  /// @brief Gets the graph of the current path finder.
  /// \return The graph of the current path finder.
  [[nodiscard]] std::shared_ptr<Graph> getGraph() const { return m_graph; }

private:
  std::shared_ptr<Graph> createGraph();
  bool inLineOfSight(glm::vec2 start, glm::vec2 end);

private:
  std::shared_ptr<Graph> m_graph;
  const std::vector<Walkbox> &m_walkboxes;
};

template<typename TContainer>
PathFinder::PathFinder(const TContainer &walkboxes)
    : m_walkboxes(walkboxes) {
}

template<typename TIterator>
PathFinder::PathFinder(TIterator begin, TIterator end)
    : m_walkboxes(begin, end) {
}

} // namespace ngf