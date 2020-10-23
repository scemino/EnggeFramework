#pragma once
#include <glm/vec2.hpp>
#include <ngf/Graphics/Color.h>

namespace ngf {
struct Vertex {
  glm::vec2 pos;
  ngf::Color color;
};
}
