#include <ngf/Graphics/Sprite.h>

namespace ngf {
void Sprite::draw(RenderTarget &target, const RenderStates &states) {
  RenderStates localStates = states;
  localStates.transform *= getTransform().getTransform();
  localStates.texture = m_texture;
  target.draw(PrimitiveType::Triangles,
              m_vertices.data(),
              m_vertices.size(),
              m_indices.data(),
              m_indices.size(),
              localStates);
}

void Sprite::updateGeometry() {
  if (!m_texture)
    return;

  auto textureSize = glm::vec2(m_texture->getSize());
  auto spriteSize = textureSize * m_textureRect.getSize();

  m_vertices[0].pos = {0.0f, 0.0f};
  m_vertices[1].pos = {spriteSize.x, 0.0f};
  m_vertices[2].pos = {spriteSize.x, spriteSize.y};
  m_vertices[3].pos = {0.f, spriteSize.y};
  m_vertices[0].texCoords = {m_textureRect.min.x, m_textureRect.max.y};
  m_vertices[1].texCoords = m_textureRect.max;
  m_vertices[2].texCoords = {m_textureRect.max.x, m_textureRect.min.y};
  m_vertices[3].texCoords = m_textureRect.min;

  m_bounds = frect::fromPositionSize({0.0f, 0.0f}, spriteSize);
}

void Sprite::setColor(const Color &color) {
  std::for_each(m_vertices.begin(), m_vertices.end(), [color](auto &vertex) { vertex.color = color; });
}

Color Sprite::getColor() const {
  return m_vertices[0].color;
}
}
