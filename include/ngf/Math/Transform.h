#pragma once
#include <glm/vec2.hpp>
#include <glm/mat3x3.hpp>
#include <ngf/Graphics/Anchor.h>
#include <ngf/Graphics/Rect.h>

namespace ngf {
/// @brief Decomposed transform defined by a position, a rotation and a scale.
class Transform {
public:
  /// @brief Sets the orientation of the object.
  /// \param angle Rotation in radians.
  void setRotation(float angle) noexcept {
    m_angle = angle;
    m_dirty = true;
  }

  /// @brief Rotates the object.
  ///
  /// This function adds to the current rotation of the object, unlike setRotation which overwrites it. Thus, it is equivalent to the following code:
  /// \param angle Rotation in radians.
  void rotate(float angle) noexcept {
    setRotation(getRotation() + angle);
  }

  /// @brief Gets the orientation of the object.
  /// \return The current rotation in radians.
  [[nodiscard]] float getRotation() const noexcept {
    return m_angle;
  }

  /// @brief Sets the position of the object.
  /// \param pos The position of the object.
  void setPosition(glm::vec2 pos) noexcept {
    m_position = pos;
    m_dirty = true;
  }

  /// @brief Gets the position of the object.
  /// \return The position of the object.
  [[nodiscard]] glm::vec2 getPosition() const noexcept {
    return m_position;
  }

  /// @brief Sets the origin of the object.
  /// \param pos The origin of the object.
  void setOrigin(glm::vec2 origin) noexcept {
    m_origin = origin;
    m_dirty = true;
  }

  /// @brief Gets the origin of the object.
  /// \return The origin of the object.
  [[nodiscard]] glm::vec2 getOrigin() const noexcept {
    return m_origin;
  }

  /// @brief Sets the origin from an anchor and bounds.
  /// \param anchor An anchor and bounds.
  /// \param bounds The bounds of the entity
  void setOriginFromAnchorAndBounds(Anchor anchor, const frect &bounds) {
    setOrigin(bounds.getPositionFromAnchor(anchor));
  }

  /// @brief Sets the scale of the object.
  /// \param scale The scale of the object.
  void setScale(glm::vec2 scale) noexcept {
    m_scale = scale;
    m_dirty = true;
  }

  /// @brief Gets the scale of the object.
  /// \return
  [[nodiscard]] glm::vec2 getScale() const noexcept {
    return m_scale;
  }

  /// @brief Gets the combined transform of the object.
  /// \return The combined transform of the object.
  [[nodiscard]] glm::mat3 getTransform() const;

private:
  mutable glm::mat3 m_transform{1};
  mutable bool m_dirty{false};
  glm::vec2 m_origin{0, 0};
  glm::vec2 m_position{0, 0};
  glm::vec2 m_scale{1, 1};
  float m_angle{0.f};
};

frect transform(const glm::mat3 &mat, const frect &rect);
glm::vec2 transform(const glm::mat3 &mat, glm::vec2 point);
}
