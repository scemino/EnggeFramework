#pragma once
#include <vector>
#include <memory>

namespace ngf {

/// @brief An edge is a part of a walkable area, it is used by a Graph.
/// @sa PathFinder, Graph.
struct GraphEdge {
  int from{0}; ///< Index of the node in the graph representing the start of the edge.
  int to{0}; ///< Index of the node in the graph representing the end of the edge.
  float cost{0.f}; ///< Cost of the edge in the graph.

  /// @brief Creates an edge with 2 indices of nodes in a graph and a cost.
  /// \param from Index of the node in the graph representing the start of the edge.
  /// \param to Index of the node in the graph representing the end of the edge.
  /// \param cost Cost of the edge in the graph.
  GraphEdge(int from, int to, float cost);
};

/// @brief Writes a representation of the edge to a stream.
/// \param os Stream where to write this representation.
/// \param edge Edge to write.
/// \return The stream where the representation has been written.
std::ostream &operator<<(std::ostream &os, const GraphEdge &edge);
}
