#pragma once
#include <cassert>
#include <ngf/Math/PathFinding/Walkbox.h>
#include <ngf/Math/Transform.h>

namespace ngf {
template<typename T, typename U>
constexpr T lerp(T lhs, T rhs, U t) {
  return (U(1) - t) * lhs + t * rhs;
}

template<typename T>
bool contains(const T& object, glm::vec2 pos){
  const auto rect = object.getLocalBounds();
  const auto trsf = object.getTransform().getTransform();

  const std::vector<glm::ivec2> v
      {{
           ngf::transform(trsf, rect.getTopLeft()),
           ngf::transform(trsf, rect.getTopRight()),
           ngf::transform(trsf, rect.getBottomRight()),
           ngf::transform(trsf, rect.getBottomLeft())
       }};

  ngf::Walkbox walkbox(v);
  return walkbox.inside(pos);
}
}