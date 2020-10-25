#pragma once
#include <glm/vec2.hpp>
#include <glm/mat3x3.hpp>
#include <ngf/Graphics/Rect.h>

namespace ngf {
class View {
public:
  View();
  explicit View(const frect &rect);
  View(const glm::vec2& center, const glm::vec2& size);

  [[nodiscard]] frect getBounds() const;

  void setCenter(glm::vec2 center) { m_center = center; }
  [[nodiscard]] glm::vec2 getCenter() const { return m_center; }

  void setSize(glm::vec2 size) { m_size = size; }
  [[nodiscard]] glm::vec2 getSize() const { return m_size; }

  void setRotation(float angle) { m_rotation = angle; }
  [[nodiscard]] float getRotation() const { return m_rotation; }

  void setViewport(const frect &viewport);
  [[nodiscard]] const frect &getViewport() const { return m_viewport; }

  void reset(const frect &rect);
  void move(const glm::vec2& offset);
  void rotate(float angle);
  void zoom(float factor);

  [[nodiscard]] glm::mat3 getTransform() const;
  [[nodiscard]] glm::mat3 getInverseTransform() const;

private:
  glm::vec2 m_center;
  glm::vec2 m_size;
  float m_rotation;
  frect m_viewport;
};
}
