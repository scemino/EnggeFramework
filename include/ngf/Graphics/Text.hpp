#pragma once
#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include <ngf/Graphics/FntFont.hpp>
#include <ngf/Graphics/Vertex.h>
#include "Transform.h"

namespace ngf {
class RenderTarget;

// This code has been copied from SFML Text and adapted to us FntFont
class Text {
public:
  Text();
  Text(std::wstring string, const Font &font, unsigned int characterSize = 30);

  void setString(const std::string &string);
  std::string getString() const;

  void setWideString(const std::wstring &string);
  std::wstring getWideString() const { return m_string; }

  void setFont(const Font &font);
  const Font *getFont() const;

  void setMaxWidth(float maxWidth);
  float getMaxWidth() const { return m_maxWidth; }

  void setColor(const Color &color);
  Color getColor() const { return m_color; }

  void draw(RenderTarget &target, const RenderStates &states = {}) const;

  frect getLocalBounds() const;
  ngf::Transform &getTransform() { return m_transform; }

private:
  void ensureGeometryUpdate() const;

private:
  ngf::Transform m_transform{};
  std::wstring m_string;
  const Font *m_font{nullptr};
  unsigned int m_characterSize{0};
  mutable Color m_color{Colors::White};
  mutable std::vector<Vertex> m_vertices;
  mutable frect m_bounds{};
  mutable bool m_geometryNeedUpdate{false};
  mutable const Texture *m_fontTexture{nullptr};
  float m_maxWidth{0};
};
}
