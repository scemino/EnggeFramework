#include <algorithm>
#include <ngf/Math/Transform.h>
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

glm::vec2 transform(const glm::mat3& mat, glm::vec2 point) {
  return { mat[0][0] * point.x + mat[0][1] * point.y + mat[0][2], mat[1][0] * point.x + mat[1][1] * point.y + mat[1][2] };
}

frect transform(const glm::mat3& mat, const frect& rect) {
  auto pointTopLeft = transform(mat, rect.getTopLeft());
  auto pointTopRight = transform(mat, rect.getTopRight());
  auto pointBottomLeft = transform(mat, rect.getBottomLeft());
  auto pointBottomRight = transform(mat, rect.getBottomRight());

  glm::vec2 min;
  glm::vec2 max;

  std::tie(min.x, max.x) = std::minmax({ pointTopLeft.x, pointTopRight.x, pointBottomLeft.x, pointBottomRight.x });
  std::tie(min.y, max.y) = std::minmax({ pointTopLeft.y, pointTopRight.y, pointBottomLeft.y, pointBottomRight.y });

  return frect::fromMinMax(min, max);
}
}