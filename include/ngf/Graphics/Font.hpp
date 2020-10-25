#pragma once
#include <ngf/Graphics/Rect.h>
#include <ngf/Graphics/Texture.h>

namespace ngf {
class Glyph {
public:
  float advance{0};
  frect bounds;
  irect textureRect;
};

class Font {
public:
  virtual ~Font() = default;
  [[nodiscard]] virtual const Glyph &getGlyph(unsigned int codePoint) const = 0;
  [[nodiscard]] virtual float getKerning(unsigned int first,
                                         unsigned int second, unsigned int characterSize) const = 0;
  [[nodiscard]] virtual const Texture &getTexture(
      unsigned int characterSize) const = 0;
};
}
