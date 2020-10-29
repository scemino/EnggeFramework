#pragma once
#include <cassert>

namespace ngf {
template<typename T, typename U>
constexpr T lerp(T lhs, T rhs, U t) {
  return (U(1) - t) * lhs + t * rhs;
}
}