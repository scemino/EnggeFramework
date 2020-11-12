#include <ngf/Graphics/Shape.h>

namespace ngf {
Shape::Shape()
    : m_textureRect(ngf::frect::fromPositionSize({0.0f, 0.0f}, {1.0f, 1.0f})) {
}

Shape::~Shape() = default;

Transform &Shape::getTransform() { return m_transform; }

const Transform &Shape::getTransform() const { return m_transform; }

void Shape::setTexture(const Texture &texture, bool resetRect) {
  m_texture = &texture;

  if (resetRect) {
    m_textureRect = ngf::frect::fromPositionSize({0.0f, 0.0f}, {1.0f, 1.0f});
    updateTexCoords();
  }
}

void Shape::setTextureRect(const frect &rect) {
  m_textureRect = rect;
  updateTexCoords();
}

void Shape::setColor(const Color &color) {
  m_color = color;
  updateColors();
}

Color Shape::getColor() const {
  return m_color;
}

frect Shape::getLocalBounds() const {
  return m_bounds;
}

void Shape::setAnchor(Anchor anchor) {
  m_transform.setOriginFromAnchorAndBounds(anchor, getLocalBounds());
}

void Shape::draw(RenderTarget &target, RenderStates states) const {
  RenderStates localStates = states;
  localStates.transform *= m_transform.getTransform();
  localStates.texture = m_texture;
  target.draw(PrimitiveType::TriangleFan, m_vertices, localStates);
}

void Shape::updateGeometry() {
  std::size_t count = getPointCount();
  assert(count >= 3);

  m_vertices.resize(count);

  auto min = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
  auto max = glm::vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

  for (std::size_t i = 0; i < count; ++i) {
    auto point = getPoint(i);
    m_vertices[i].pos = point;
    min = ngf::min(min, point);
    max = ngf::max(max, point);
  }

  updateColors();
  updateTexCoords();
}

void Shape::updateBounds(frect bounds) {
  m_bounds = bounds;
  updateTexCoords();
}

void Shape::updateColors() {
  for (auto &l_vertex : m_vertices) {
    l_vertex.color = m_color;
  }
}

void Shape::updateTexCoords() {
  for (auto &l_vertex : m_vertices) {
    glm::vec2 ratio(0.0f, 0.0f);

    if (!m_bounds.isEmpty()) {
      ratio = (l_vertex.pos - m_bounds.getPosition()) / m_bounds.getSize();
    }

    l_vertex.texCoords = m_textureRect.getPosition() + m_textureRect.getSize() * ratio;
  }
}
}