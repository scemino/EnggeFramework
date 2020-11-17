/* Part of this file comes from Gamedev Framework (gf)
 *
 * Gamedev Framework (gf)
 * Copyright (C) 2016-2019 Julien Bernard
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Part of this file comes from SFML, with the same license:
 * Copyright (C) 2007-2015 Laurent Gomila (laurent@sfml-dev.org)
 */

#pragma once
#include <vector>
#include <list>
#include <filesystem>
#include <memory>
#include <iostream>
#include <map>
#include <ngf/Graphics/Font.h>

namespace ngf {

class DefaultFont : public Font {
private:
  struct Packing {
    int top = 0;
    int bottom = 0;
    int right = 0;
  };

  struct GlyphCache {
    std::unique_ptr<Texture> texture;
    std::map<uint64_t, Glyph> glyphs;
    Packing packing;
  };

  void *m_library{nullptr};
  void *m_stroker{nullptr};
  void *m_face{nullptr};
  unsigned int m_currentCharacterSize{0};
  std::map<unsigned, GlyphCache> m_cache;

public:
  DefaultFont();
  DefaultFont(const DefaultFont &) = delete;
  explicit DefaultFont(std::istream &stream);
  explicit DefaultFont(const std::filesystem::path &filename);
  DefaultFont(uint8_t *content, size_t size);
  DefaultFont(DefaultFont &&other) noexcept;
  ~DefaultFont() override;

  DefaultFont &operator=(const DefaultFont &) = delete;
  DefaultFont &operator=(DefaultFont &&other) noexcept;

  void loadFromFile(const std::filesystem::path &path);

  [[nodiscard]] const Glyph &getGlyph(unsigned int codePoint,
                                      unsigned int characterSize,
                                      float outlineThickness) override;
  [[nodiscard]] float getKerning(unsigned int first, unsigned int second,
                                 unsigned int characterSize) override;
  [[nodiscard]] const Texture *getTexture(unsigned int characterSize) override;
  [[nodiscard]] float getLineSpacing(unsigned int) override;
  void generateTexture(unsigned int characterSize);

private:
  GlyphCache createCache(unsigned int characterSize);
  Glyph createGlyph(unsigned int codepoint, unsigned int characterSize, float outlineThickness, GlyphCache &cache);
  bool setCurrentCharacterSize(unsigned int characterSize);
};
}
