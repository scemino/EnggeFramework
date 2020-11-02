#include <cassert>
#include <glm/gtx/transform.hpp>
#include "View.h"
#include <ngf/Window/Window.h>

namespace ngf {
namespace {
bool isClamped(float value) {
  return 0.0f <= value && value <= 1.0f;
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

glm::mat3 View::getTransform() const {
  glm::vec2 factors = Window::getSizeScale() * 2.0f / m_size;
  return glm::translate(
      glm::rotate(
          glm::scale(glm::mat4(1),
                     glm::vec3({factors.x, -factors.y, 1.f})), -m_rotation, glm::vec3(0.f, 0.f, 1.f)),
      glm::vec3(-m_center, 1.f));
}

glm::mat3 View::getInverseTransform() const {
  return glm::inverse(getTransform());
}
}
