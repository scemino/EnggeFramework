#pragma once
#include <ngf/Graphics/Anchor.h>
#include <glm/vec2.hpp>

namespace ngf {
template<typename T>
struct Rect {
private:
  using vec = glm::vec<2, T, glm::defaultp>;

public:
  constexpr Rect() noexcept = default;

  static constexpr Rect<T> fromMinMax(vec min, vec max) noexcept {
    return Rect<T>(min, max);
  }

  static constexpr Rect<T> fromPositionSize(vec position, vec size) noexcept {
    return Rect<T>(position, position + size);
  }

  constexpr vec getPosition() const noexcept {
    return min;
  }

  constexpr vec getPositionFromAnchor(Anchor anchor) const noexcept {
    switch (anchor) {
    case Anchor::TopLeft:
      return min;
    case Anchor::TopCenter:
      return { (min.x + max.x) / T(2), min.y };
    case Anchor::TopRight:
      return { max.x, min.y };
    case Anchor::CenterLeft:
      return { min.x, (min.y + max.y) / T(2) };
    case Anchor::Center:
      return getCenter();
    case Anchor::CenterRight:
      return { max.x, (min.y + max.y) / T(2) };
    case Anchor::BottomLeft:
      return { min.x, max.y };
    case Anchor::BottomCenter:
      return { (min.x + max.x) / T(2), max.y };
    case Anchor::BottomRight:
      return max;
    }
    return min;
  }

  constexpr vec getSize() const noexcept {
    return max - min;
  }

  constexpr vec getCenter() const noexcept {
    return min + (max - min) / static_cast<T>(2);
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