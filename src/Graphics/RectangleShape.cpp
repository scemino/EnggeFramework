#include <ngf/Graphics/RectangleShape.h>

namespace ngf {
RectangleShape::RectangleShape(glm::vec2 size)
    : m_size(size) {
  updateGeometry();
  updateBounds(ngf::frect::fromPositionSize({0.0f, 0.0f}, m_size));
}

RectangleShape::RectangleShape(const frect &rect)
    : m_size(rect.getSize()) {
  updateGeometry();
  updateBounds(ngf::frect::fromPositionSize({0.0f, 0.0f}, m_size));
  getTransform().setPosition(rect.getPosition());
}

void RectangleShape::setSize(glm::vec2 size) {
  if (m_size == size) {
    return;
  }
  m_size = size;
  updateGeometry();
  updateBounds(ngf::frect::fromPositionSize({0.0f, 0.0f}, m_size));
}

std::size_t RectangleShape::getPointCount() const {
  return 4;
}

glm::vec2 RectangleShape::getPoint(std::size_t index) const {
  switch (index) {
  case 0:return {0.0f, 0.0f};
  case 1:return {m_size.x, 0.0f};
  case 2:return {m_size.x, m_size.y};
  case 3:return {0.0f, m_size.y};
  }
  return {0.0f, 0.0f};
}
}