#pragma once
#include <vector>
#include <glm/vec2.hpp>

namespace ngf {
class Walkbox;
class Graph;

class PathFinder {
public:
  template<typename TContainer>
  explicit PathFinder(const TContainer &walkboxes);
  template<typename TIterator>
  PathFinder(TIterator begin, TIterator end);

  std::vector<glm::vec2> calculatePath(glm::vec2 from, glm::vec2 to);
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