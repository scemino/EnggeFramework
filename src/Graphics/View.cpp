#include <cassert>
#include <glm/gtx/transform.hpp>
#include <ngf/Graphics/View.h>

namespace ngf {
namespace {
bool isClamped(float value) {
  return 0.0f <= value && value <= 1.0f;
}

inline glm::mat3 rotation(float angle) {
  float cos = std::cos(angle);
  float sin = std::sin(angle);
  return glm::mat3{
      cos, -sin, 0.0f,
      sin, cos, 0.0f,
      0.0f, 0.0f, 1.0f
  };
}

inline glm::mat3 translation(glm::vec2 offset) {
  return glm::mat3{
      1.0f, 0.0f, offset.x,
      0.0f, 1.0f, offset.y,
      0.0f, 0.0f, 1.0f
  };
}

inline glm::mat3 scaling(glm::vec2 factor) {
  return glm::mat3{
      factor.x, 0.0f, 0.0f,
      0.0f, factor.y, 0.0f,
      0.0f, 0.0f, 1.0f
  };
}
}

View::View()
    : m_center{0.0f, 0.0f}, m_size{1000.0f, 1000.0f}, m_rotation(0),
      m_viewport(frect::fromPositionSize({0.0f, 0.0f}, {1.0f, 1.0f})) {
}

View::View(const frect &rect)
    : m_center(rect.getCenter()), m_size(rect.getSize()), m_rotation(0),
      m_viewport(frect::fromPositionSize({0.0f, 0.0f}, {1.0f, 1.0f})) {
}

View::View(const glm::vec2 &center, const glm::vec2 &size)
    : m_center(center), m_size(size), m_rotation(0), m_viewport(frect::fromPositionSize({0.0f, 0.0f}, {1.0f, 1.0f})) {
}

frect View::getBounds() const {
  return frect::fromCenterSize(m_center, m_size);
}

void View::setViewport(const frect &viewport) {
  assert(isClamped(viewport.min.x));
  assert(isClamped(viewport.min.y));
  assert(isClamped(viewport.max.x));
  assert(isClamped(viewport.max.y));

  m_viewport = viewport;
}

void View::reset(const frect &rect) {
  m_center = rect.getCenter();
  m_size = rect.getSize();
}

void View::rotate(float angle) {
  m_rotation += angle;
}

void View::move(const glm::vec2 &offset) {
  m_center += offset;
}

void View::zoom(float factor) {
  m_size *= factor;
}

void View::zoom(float factor, glm::vec2 fixed) {
  m_center += (fixed - m_center) * (1 - factor);
  m_size *= factor;
}

glm::mat3 View::getTransform() const {
  glm::vec2 factors = 2.0f / m_size;
  return translation(-m_center) * rotation(-m_rotation) * scaling({factors.x, -factors.y});
}

glm::mat3 View::getInverseTransform() const {
  return glm::inverse(getTransform());
}
}
