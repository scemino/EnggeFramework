#pragma once
#include <glm/vec2.hpp>

namespace ngf {
struct Segment {
public:
  Segment() = default;
  Segment(const glm::vec2 &start, const glm::vec2 &end);

  void normalize();

  [[nodiscard]] float distance(glm::vec2 p) const;

  glm::vec2 start{};
  glm::vec2 end{};
  float left{0}, right{0}, top{0}, bottom{0};
  float a{0}, b{0}, c{0};
};

bool operator==(Segment lhs, const Segment &rhs);
bool operator!=(Segment lhs, const Segment &rhs);
}

