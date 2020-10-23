#pragma once
#include <glm/vec2.hpp>
#include <glm/mat3x3.hpp>

namespace ngf {
class Transform {
public:
  void setRotation(float angle) noexcept {
    m_angle = angle;
    m_dirty = true;
  }
  [[nodiscard]] float getRotation() const noexcept {
    return m_angle;
  }
  void setPosition(glm::vec2 pos) noexcept {
    m_position = pos;
    m_dirty = true;
  }
  [[nodiscard]] glm::vec2 getPosition() const noexcept {
    return m_position;
  }
  void setOrigin(glm::vec2 origin) noexcept {
    m_origin = origin;
    m_dirty = true;
  }
  [[nodiscard]] glm::vec2 getOrigin() const noexcept {
    return m_origin;
  }
  void setScale(glm::vec2 scale) noexcept {
    m_scale = scale;
    m_dirty = true;
  }
  [[nodiscard]] glm::vec2 getScale() const noexcept {
    return m_scale;
  }

  [[nodiscard]] glm::mat3 getTransform() const;

private:
  mutable glm::mat3 m_transform{1};
  mutable bool m_dirty{false};
  glm::vec2 m_origin{0, 0};
  glm::vec2 m_position{0, 0};
  glm::vec2 m_scale{1, 1};
  float m_angle{0.f};
};
}
