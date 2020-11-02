#pragma once
#include <cassert>
#include <ngf/Math/PathFinding/Walkbox.h>
#include <ngf/Math/Transform.h>

namespace ngf {
/// @brief Lerp, or Linear Interpolation, is a mathematical function that returns a value between two others at a point on a linear scale.
/// \tparam T Type of the value to interpolate.
/// \tparam U
/// \param a Value from where to start the linear interpolation.
/// \param b Value to where linear interpolation ends.
/// \param t The interpolation point is a value between 0 and 1. Essentially, acting as a percentage between a and b.
/// \return The interpolated value.
template<typename T, typename U>
constexpr T lerp(T a, T b, U t) {
  return (U(1) - t) * a + t * b;
}

/// @brief Indicates whether or not the position is inside the specified object.
/// \tparam T Type of the object.
/// \param object Object to test if the position is inside it.
/// \param position Position to test.
/// \return true if the position is inside the specified object, otherwise false.
template<typename T>
bool contains(const T& object, glm::vec2 position){
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
  return walkbox.inside(position);
}
}