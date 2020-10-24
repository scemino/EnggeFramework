#pragma once
#include <glm/vec2.hpp>

namespace ngf {

template<typename T>
struct Rect {
private:
  using vec = glm::vec<2, T, glm::defaultp>;

public:
  constexpr Rect() noexcept {
  }

  static constexpr Rect<T> fromMinMax(vec min, vec max) noexcept {
    return Rect<T>(min, max);
  }

  static constexpr Rect<T> fromPositionSize(vec position, vec size) noexcept {
    return Rect<T>(position, position + size);
  }

  constexpr vec getPosition() const noexcept {
    return min;
  }

  constexpr vec getSize() const noexcept {
    return max - min;
  }

private:
  constexpr Rect(vec min, vec max) noexcept
      : min(min), max(max) {
  }

public:
  glm::vec<2, T, glm::defaultp> min;
  glm::vec<2, T, glm::defaultp> max;
};

using frect = Rect<float>;
using irect = Rect<int>;

template<typename T>
inline bool operator==(const Rect<T>& lhs, const Rect<T>& rhs) {
  return lhs.min == rhs.min && lhs.max == rhs.max;
}

template<typename T>
inline bool operator!=(const Rect<T>& lhs, const Rect<T>& rhs) {
  return !(lhs == rhs);
}

}