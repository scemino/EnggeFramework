#pragma once
#include <ngf/Graphics/Anchor.h>
#include <glm/vec2.hpp>

namespace ngf {
/// @brief Axis-aligned rectangle.
template<typename T>
struct Rect {
private:
  using vec = glm::vec<2, T, glm::defaultp>;

public:
  /// Creates an empty rectangle located at the origin.
  /// @note Don't use default constructor, compilation fails with Ubuntu
  constexpr Rect() noexcept {}

  /// @brief Create a rectangle from a minimum point and a maximum point.
  /// \param min The minimum point in the rectangle.
  /// \param max The maximum point in the rectangle.
  /// \return The rectangle created.
  static constexpr Rect<T> fromMinMax(vec min, vec max) noexcept {
    return Rect<T>(min, max);
  }

  /// @brief Creates a rectangle from a position (top-left) and a size.
  /// \param position The top-left position of the rectangle.
  /// \param size The size of the rectangle.
  /// \return The rectangle created.
  static constexpr Rect<T> fromPositionSize(vec position, vec size) noexcept {
    return Rect<T>(position, position + size);
  }

  /// @brief Creates a rectangle from a center and a size.
  /// \param center The center of the rectangle.
  /// \param size The size of the rectangle.
  /// \return The rectangle created.
  static constexpr Rect<T> fromCenterSize(vec center, vec size) noexcept {
    return Rect<T>(center - size / T(2), center + size / T(2));
  }

  /// @brief Gets the position of the rectangle.
  /// \return the position of the rectangle.
  constexpr vec getPosition() const noexcept { return min; }

  /// @brief Gets a value indicating if the rectangle is empty.
  /// \return true if the rectangle is empty.
  [[nodiscard]] constexpr bool isEmpty() const noexcept {
    return min.x >= max.x || min.y >= max.y;
  }

  /// @brief Gets a position from the rectangle and an anchor.
  constexpr vec getPositionFromAnchor(Anchor anchor) const noexcept {
    switch (anchor) {
    case Anchor::TopLeft:return min;
    case Anchor::TopCenter:return {(min.x + max.x) / T(2), min.y};
    case Anchor::TopRight:return {max.x, min.y};
    case Anchor::CenterLeft:return {min.x, (min.y + max.y) / T(2)};
    case Anchor::Center:return getCenter();
    case Anchor::CenterRight:return {max.x, (min.y + max.y) / T(2)};
    case Anchor::BottomLeft:return {min.x, max.y};
    case Anchor::BottomCenter:return {(min.x + max.x) / T(2), max.y};
    case Anchor::BottomRight:return max;
    }
    return min;
  }

  /// @brief Gets the size of the rectangle.
  /// \return The size of the rectangle.
  constexpr vec getSize() const noexcept { return max - min; }

  /// @brief Gets the center of the rectangle.
  /// \return The center of the rectangle.
  constexpr vec getCenter() const noexcept {
    return min + (max - min) / static_cast<T>(2);
  }

  /// @brief Gets the width of the rectangle.
  /// \return The width of the rectangle.
  constexpr T getWidth() const noexcept {
    return this->max.x - this->min.x;
  }

  /// @brief Gets the height of the rectangle.
  /// \return The height of the rectangle.
  constexpr T getHeight() const noexcept {
    return this->max.y - this->min.y;
  }

  /// @brief Get the top left corner of the rectangle.
  /// \return The top left corner of the rectangle.
  constexpr vec getTopLeft() const noexcept {
    return getPositionFromAnchor(Anchor::TopLeft);
  }

  /// @brief Get the top right corner of the rectangle.
  /// \return The top right corner of the rectangle.
  constexpr vec getTopRight() const noexcept {
    return getPositionFromAnchor(Anchor::TopRight);
  }

  /// @brief Get the bottom left corner of the rectangle.
  /// \return The bottom left corner of the rectangle.
  constexpr vec getBottomLeft() const noexcept {
    return getPositionFromAnchor(Anchor::BottomLeft);
  }

  /// @brief Get the bottom right corner of the rectangle.
  /// \return The bottom right corner of the rectangle.
  constexpr vec getBottomRight() const noexcept {
    return getPositionFromAnchor(Anchor::BottomRight);
  }

  /// @brief Indicates whether or not a point is inside the rectangle.
  /// \param point The point to test.
  /// \return true if the point is inside the rectangle, false otherwise.
  constexpr bool contains(vec point) const noexcept {
    return min.x <= point.x && point.x < max.x && min.y <= point.y && point.y < max.y;
  }

  /// @brief Shrinks the rectangle.
  /// \param value The amount to shrink.
  /// \return The rectangle shrinked.
  constexpr Rect shrink(T value) const noexcept {
    return Rect(min + value, max - value);
  }

private:
  constexpr Rect(vec min, vec max) noexcept
      : min(min), max(max) {
  }

public:
  glm::vec<2, T, glm::defaultp> min{0, 0};
  glm::vec<2, T, glm::defaultp> max{0, 0};
};

using frect = Rect<float>;
using irect = Rect<int>;

/// @brief Compares 2 rectangles for equality.
/// \param lhs The left rectangle operand.
/// \param rhs The right rectangle operand.
/// \return true if they are equals, false otherwise.
template<typename T>
inline bool operator==(const Rect<T> &lhs, const Rect<T> &rhs) {
  return lhs.min == rhs.min && lhs.max == rhs.max;
}

/// @brief Compares 2 rectangles for inequality.
/// \param lhs The left rectangle operand.
/// \param rhs The right rectangle operand.
/// \return true if they are not equals, false otherwise.
template<typename T>
inline bool operator!=(const Rect<T> &lhs, const Rect<T> &rhs) {
  return !(lhs == rhs);
}

}