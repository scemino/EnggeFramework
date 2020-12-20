#pragma once
#include <ngf/Graphics/Drawable.h>
#include <ngf/Graphics/Sprite.h>
#include <ngf/Graphics/Texture.h>
#include <ngf/Math/Transform.h>
#include <glm/vec2.hpp>
#include "Camera.h"
#include "SpriteSheetItem.h"
#include "LightingShader.h"

class Layer final : public ngf::Drawable {
public:
  Layer(int offsetY, int roomSizeY, std::shared_ptr<ngf::Texture> texture,
        std::vector<SpriteSheetItem> items,
        glm::vec2 parallax,
        float zsort)
      : m_offsetY(offsetY), m_roomSizeY(roomSizeY), m_texture(std::move(texture)), m_items(std::move(items)),
        m_parallax(parallax), m_zsort(zsort) {
  }
  ~Layer() override = default;

  void setVisible(bool visible) { m_visible = visible; }
  bool isVisible() const { return m_visible; }

  const std::vector<SpriteSheetItem> &getItems() const { return m_items; }

  void setParallax(const glm::vec2 &parallax) { m_parallax = parallax; }
  glm::vec2 getParallax() const { return m_parallax; }

  ngf::Transform &getTransform() { return m_transform; }

  void setZSort(float zsort) { m_zsort = zsort; }
  float getZSort() const { return m_zsort; }

  void draw(ngf::RenderTarget &target, ngf::RenderStates states) const override {
    if (!m_visible)
      return;

    states.transform *= m_transform.getTransform();
    float offsetX = 0.f;
    auto texSize = m_texture->getSize();
    for (const auto &item : m_items) {
      auto pShader = (LightingShader *) states.shader;
      pShader->setTexture(*m_texture);
      pShader->setContentSize(item.sourceSize);
      pShader->setSpriteOffset({0, -item.frame.getHeight()});
      pShader->setSpritePosInSheet({static_cast<float>(item.frame.min.x) / texSize.x,
                                    static_cast<float>(item.frame.min.y) / texSize.y});
      pShader->setSpriteSizeRelToSheet({static_cast<float>(item.sourceSize.x) / texSize.x,
                                        static_cast<float>(item.sourceSize.y) / texSize.y});

      auto sprite = std::make_unique<ngf::Sprite>(*m_texture, item.frame);
      glm::vec2 off{item.spriteSourceSize.min.x, item.spriteSourceSize.min.y + m_roomSizeY - item.sourceSize.y};
      sprite->getTransform().setPosition(off + glm::vec2{offsetX, m_offsetY});
      offsetX += item.frame.getWidth();
      sprite->draw(target, states);
    }
  }

private:
  std::string m_name;
  bool m_visible{true};
  const int m_offsetY{0};
  std::shared_ptr<ngf::Texture> m_texture;
  std::vector<SpriteSheetItem> m_items;
  glm::vec2 m_parallax{1.f, 1.f};
  float m_zsort{0.f};
  int m_roomSizeY{0};
  ngf::Transform m_transform;
};