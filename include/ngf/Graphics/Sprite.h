#pragma once
#include <array>
#include <ngf/Graphics/Rect.h>
#include <ngf/Graphics/RenderStates.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Texture.h>
#include <ngf/Graphics/Transform.h>
#include <ngf/Graphics/Vertex.h>

namespace ngf {

namespace {
frect normalize(const Texture &texture, const irect &size) {
  auto textureSize = glm::vec(texture.getSize());
  return frect::fromMinMax(glm::vec2(static_cast<float>(size.min.x) / textureSize.x,
                                     static_cast<float>(size.min.y) / textureSize.y),
                           glm::vec2(static_cast<float>(size.max.x) / textureSize.x,
                                     static_cast<float>(size.max.y) / textureSize.y));
}
}

class Sprite {
public:
  Sprite() = default;

  explicit Sprite(const Texture &texture)
      : m_texture(&texture) {
    updateGeometry();
  }

  Sprite(const Texture &texture, const irect &textureRect)
      : m_texture(&texture), m_textureRect(normalize(texture, textureRect)) {
    updateGeometry();
  }

  Sprite(const Texture &texture, const frect &textureRect)
      : m_texture(&texture), m_textureRect(textureRect) {
    updateGeometry();
  }

  void setColor(const Color &color);
  Color getColor() const;

  Transform &getTransform() { return m_transform; }

  void draw(RenderTarget &target, const RenderStates &states);

private:
  void updateGeometry();

private:
  const Texture *m_texture{nullptr};
  frect m_textureRect{frect::fromMinMax({0.f, 0.f}, {1.f, 1.f})};
  frect m_bounds{};
  std::array<Vertex, 4> m_vertices{};
  std::array<std::uint16_t, 6> m_indices{{0, 1, 2, 0, 2, 3}};
  Transform m_transform;
};
}
