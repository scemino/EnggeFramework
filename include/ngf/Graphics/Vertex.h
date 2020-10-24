#pragma once
#include <glm/vec2.hpp>
#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/Colors.h>

namespace ngf {
struct Vertex {
  glm::vec2 pos{0,0};
  ngf::Color color{ngf::Colors::White};
  glm::vec2 texCoords{0,0};
};
}
