#include <ngf/Graphics/Shape.h>

namespace ngf {

namespace {
// Compute the normal of a segment
glm::vec2 computeNormal(const glm::vec2 &p1, const glm::vec2 &p2) {
  glm::vec2 normal(p1.y - p2.y, p2.x - p1.x);
  return glm::normalize(normal);
}
}

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

  if (m_outlineThickness > 0.0f) {
    localStates.texture = nullptr;
    target.draw(PrimitiveType::TriangleStrip, m_outlineVertices, localStates);
  }
}

void Shape::updateGeometry() {
  std::size_t count = getPointCount();
  assert(count >= 3);

  m_vertices.resize(count + 2);

  auto min = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
  auto max = glm::vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

  for (std::size_t i = 0; i < count; ++i) {
    auto point = getPoint(i);
    m_vertices[i + 1].pos = point;
    min = ngf::min(min, point);
    max = ngf::max(max, point);
  }

  m_vertices[count + 1].pos = m_vertices[1].pos;
  m_vertices[0].pos = (min + max) / 2.f;

  updateColors();
  updateTexCoords();
  updateOutline();
}

void Shape::updateBounds(frect bounds) {
  m_bounds = bounds;
  updateTexCoords();
}

void Shape::updateAutoBounds() {
  if (m_vertices.empty()) {
    m_bounds = frect();
  } else {
    auto min = m_vertices[0].pos;
    auto max = m_vertices[0].pos;
    for (const Vertex &vertex : m_vertices) {
      min = ngf::min(min, vertex.pos);
      max = ngf::max(max, vertex.pos);
    }
    m_bounds = frect::fromMinMax(min, max);
  }
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

void Shape::setOutlineColor(const Color &color) {
  m_outlineColor = color;
  updateOutlineColors();
}

void Shape::setOutlineThickness(float thickness) {
  m_outlineThickness = thickness;
  updateOutline();
}

void Shape::updateOutline() {
  if (m_outlineThickness <= 0.0f)
    return;

  std::size_t count = getPointCount();
  m_outlineVertices.resize((count + 1) * 2);

  auto center = m_vertices[0].pos;

  for (std::size_t i = 0; i < count; ++i) {
    std::size_t j = i + 1;

    auto prev = (i == 0) ? m_vertices[count].pos : m_vertices[j - 1].pos;
    auto curr = m_vertices[j].pos;
    auto next = m_vertices[j + 1].pos;

    auto normalPrev = computeNormal(prev, curr);
    auto normalNext = computeNormal(curr, next);

    if (dot(normalPrev, center - curr) > 0) {
      normalPrev = -normalPrev;
    }

    if (dot(normalNext, center - curr) > 0) {
      normalNext = -normalNext;
    }

    float factor = 1.0f + dot(normalPrev, normalNext);
    auto normal = (normalPrev + normalNext) / factor;

    m_outlineVertices[i * 2 + 0].pos = curr;
    m_outlineVertices[i * 2 + 1].pos = curr + normal * m_outlineThickness;
  }

  m_outlineVertices[count * 2 + 0].pos = m_outlineVertices[0].pos;
  m_outlineVertices[count * 2 + 1].pos = m_outlineVertices[1].pos;

  updateOutlineColors();
}

void Shape::updateOutlineColors() {
  for (auto &m_outlineVertice : m_outlineVertices) {
    m_outlineVertice.color = m_outlineColor;
  }
}
}