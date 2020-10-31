#pragma once
#include <glm/vec2.hpp>
#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/Colors.h>

namespace ngf {
/// @brief This is a point in 2D with a color and texture coordinates.
struct Vertex {
  glm::vec2 pos{0,0};           ///< Position of the vertex in world coordinates.
  ngf::Color color{ngf::Colors::White}; ///< Color of the vertex.
  glm::vec2 texCoords{0,0};     ///< Coordinates of the texture.
};
}
