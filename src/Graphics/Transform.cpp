#include <ngf/Graphics/Transform.h>
#include <glm/gtx/matrix_operation.hpp>

namespace ngf {
[[nodiscard]] glm::mat3 Transform::getTransform() const {
  if (!m_dirty)
    return m_transform;
  auto rotation = glm::radians(m_angle);
  float cos = std::cos(rotation);
  float sin = std::sin(rotation);
  float xx = m_scale.x * cos;
  float xy = -m_scale.y * sin;
  float yx = m_scale.x * sin;
  float yy = m_scale.y * cos;
  float xz = -m_origin.x * xx - m_origin.y * xy + m_position.x;
  float yz = -m_origin.x * yx - m_origin.y * yy + m_position.y;
  m_transform = glm::mat3(xx, xy, xz, yx, yy, yz, 0.0f, 0.0f, 1.0f);
  m_dirty = false;
  return m_transform;
}
}