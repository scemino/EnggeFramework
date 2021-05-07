#include <algorithm>
#include <ngf/Graphics/Sprite.h>

namespace ngf {

namespace {
frect normalize(const Texture &texture, const irect &size) {
  auto textureSize = glm::vec(texture.getSize());
  return frect::fromMinMax(
      glm::vec2(static_cast<float>(size.min.x) / textureSize.x,
                static_cast<float>(size.min.y) / textureSize.y),
      glm::vec2(static_cast<float>(size.max.x) / textureSize.x,
                static_cast<float>(size.max.y) / textureSize.y));
}
} // namespace

Sprite::Sprite(const Texture &texture) : m_texture(&texture) {
  updateGeometry();
}

Sprite::Sprite(const Texture &texture, const irect &textureRect)
    : m_texture(&texture), m_textureRect(normalize(texture, textureRect)) {
  updateGeometry();
}

Sprite::Sprite(const Texture &texture, const frect &textureRect)
    : m_texture(&texture), m_textureRect(textureRect) {
  updateGeometry();
}

void Sprite::setTexture(const Texture &texture, bool resetRect) {
  m_texture = &texture;
  if (resetRect) {
    m_textureRect = frect::fromPositionSize({0.0f, 0.0f}, {1.0f, 1.0f});
  }
  updateGeometry();
}

void Sprite::setTextureRect(irect rect) {
  if (!m_texture)
    return;
  m_textureRect = normalize(*m_texture, rect);
  updateGeometry();
}

void Sprite::draw(RenderTarget &target, RenderStates states) const {
  states.transform *= getTransform().getTransform();
  states.texture = m_texture;
  target.draw(PrimitiveType::TriangleFan, m_vertices, states);
}

void Sprite::updateGeometry() {
  if (!m_texture)
    return;

  auto textureSize = glm::vec2(m_texture->getSize());
  auto spriteSize = textureSize * m_textureRect.getSize();

  m_vertices[0].pos = {0.0f, spriteSize.y};
  m_vertices[1].pos = {spriteSize.x, spriteSize.y};
  m_vertices[2].pos = {spriteSize.x, 0.0f};
  m_vertices[3].pos = {0.0f, 0.0f};
  m_vertices[0].texCoords = {m_flipX ? m_textureRect.max.x : m_textureRect.min.x, m_flipY ? m_textureRect.min.y : m_textureRect.max.y};
  m_vertices[1].texCoords = {m_flipX ? m_textureRect.min.x : m_textureRect.max.x, m_flipY ? m_textureRect.min.y : m_textureRect.max.y};
  m_vertices[2].texCoords = {m_flipX ? m_textureRect.min.x : m_textureRect.max.x, m_flipY ? m_textureRect.max.y : m_textureRect.min.y};
  m_vertices[3].texCoords = {m_flipX ? m_textureRect.max.x : m_textureRect.min.x, m_flipY ? m_textureRect.max.y : m_textureRect.min.y};

  m_bounds = frect::fromPositionSize({0.0f, 0.0f}, spriteSize);
}

void Sprite::setColor(const Color &color) {
  std::for_each(m_vertices.begin(), m_vertices.end(),
                [color](auto &vertex) { vertex.color = color; });
}

Color Sprite::getColor() const { return m_vertices[0].color; }

void Sprite::setAnchor(Anchor anchor) {
  m_transform.setOriginFromAnchorAndBounds(anchor, getLocalBounds());
}

void Sprite::setFlipX(bool flip) {
  if(m_flipX == flip) return;
  m_flipX = flip;
  updateGeometry();
}

bool Sprite::getFlipX() const { return m_flipX; }

void Sprite::setFlipY(bool flip) {
  if(m_flipY == flip) return;
  m_flipY = flip;
  updateGeometry();
}

bool Sprite::getFlipY() const { return m_flipY; }

} // namespace ngf
