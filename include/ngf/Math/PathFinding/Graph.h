#pragma once
#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include <ngf/Math/PathFinding/GraphEdge.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/RenderStates.h>

namespace ngf {

/// @brief A graph helps to find a path between two points.
///
/// This class has been ported from http://www.groebelsloot.com/2016/03/13/pathfinding-part-2/
/// and modified
/// Here is the original comment:
/// All pathfinding classes are based on the ActionScript 3 implementation by Eduardo Gonzalez
/// Code is ported to HaXe and modified when needed
/// http://code.tutsplus.com/tutorials/artificial-intelligence-series-part-1-path-finding--active-4439
class Graph {
public:
  std::vector<glm::vec2> nodes;
  std::vector<std::vector<std::shared_ptr<GraphEdge>>> edges;
  std::vector<glm::ivec2> concaveVertices;

public:
  /// @brief Gets the edge from 'from' index to 'to' index.
  /// \param from Index of the 'from' point.
  /// \param to Index of the 'to' point.
  /// \return The edge from 'from' index to 'to' index.
  std::shared_ptr<GraphEdge> getEdge(int from, int to);
  /// @brief Adds a node to the graph.
  /// \param node Node to add.
  /// \return
  int addNode(glm::vec2 node);
  /// @brief Adds an edge to the graph.
  /// \param edge Edge to add to the graph.
  void addEdge(const std::shared_ptr<GraphEdge> &edge);
};

} // namespace ng
