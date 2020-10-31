#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Text.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/System/StringHelper.h>
#include <utility>

namespace ngf {
namespace {
glm::vec2 normalize(const Texture &texture, const glm::ivec2 &v) {
  auto textureSize = glm::vec(texture.getSize());
  return glm::vec2(static_cast<float>(v.x) / textureSize.x,
                   static_cast<float>(v.y) / textureSize.y);
}

// Add a glyph quad to the vertex array
void addGlyphQuad(std::vector<Vertex> &vertices, glm::vec2 position,
                  const Color &color, const Glyph &glyph, const Texture &texture) {
  int padding = 1;

  float left = glyph.bounds.min.x - static_cast<float>(padding);
  float top = glyph.bounds.min.y - static_cast<float>(padding);
  float right = glyph.bounds.max.x + static_cast<float>(padding);
  float bottom = glyph.bounds.max.y + static_cast<float>(padding);

  int u1 = glyph.textureRect.min.x - padding;
  int v1 = glyph.textureRect.min.y - padding;
  int u2 = glyph.textureRect.max.x + padding - 1;
  int v2 = glyph.textureRect.max.y + padding - 1;

  vertices.push_back(
      Vertex{{position.x + left, position.y + top}, color, normalize(texture, {u1, v1})});
  vertices.push_back(
      Vertex{{position.x + right, position.y + top}, color, normalize(texture, {u2, v1})});
  vertices.push_back(
      Vertex{{position.x + left, position.y + bottom}, color, normalize(texture, {u1, v2})});
  vertices.push_back(
      Vertex{{position.x + left, position.y + bottom}, color, normalize(texture, {u1, v2})});
  vertices.push_back(
      Vertex{{position.x + right, position.y + top}, color, normalize(texture, {u2, v1})});
  vertices.push_back(
      Vertex{{position.x + right, position.y + bottom}, color, normalize(texture, {u2, v2})});
}
} // namespace

Text::Text() = default;

Text::Text(std::wstring string, const Font &font, unsigned int characterSize)
    : m_string(std::move(string)), m_font(&font),
      m_characterSize(characterSize), m_geometryNeedUpdate(true) {
}

void Text::setWideString(const std::wstring &string) {
  if (m_string != string) {
    m_string = string;
    m_geometryNeedUpdate = true;
  }
}

void Text::setString(const std::string &string) {
  setWideString(StringHelper::towstring(string));
}

std::string Text::getString() const {
  return StringHelper::tostring(m_string);
}

void Text::setFont(const Font &font) {
  if (m_font != &font) {
    m_font = &font;
    m_geometryNeedUpdate = true;
  }
}

void Text::setMaxWidth(float maxWidth) {
  if (m_maxWidth != maxWidth) {
    m_maxWidth = maxWidth;
    m_geometryNeedUpdate = true;
  }
}

void Text::setColor(const Color &color) {
  if (m_color != color) {
    m_color = color;
    m_geometryNeedUpdate = true;
  }
}

const Font *Text::getFont() const {
  return m_font;
}

frect Text::getLocalBounds() const {
  ensureGeometryUpdate();

  return m_bounds;
}

void Text::draw(RenderTarget &target, RenderStates states) const {
  if (m_font) {
    ensureGeometryUpdate();

    RenderStates s = states;
    s.texture = m_fontTexture;
    target.draw(PrimitiveType::Triangles, m_vertices.data(), m_vertices.size(), s);
  }
}

void Text::ensureGeometryUpdate() const {
  if (!m_font)
    return;

  // Do nothing, if geometry has not changed and the font texture has not
  // changed
  if (!m_geometryNeedUpdate &&
      &m_font->getTexture(m_characterSize) == m_fontTexture)
    return;

  // Save the current fonts texture id
  m_fontTexture = &m_font->getTexture(m_characterSize);

  // Mark geometry as updated
  m_geometryNeedUpdate = false;

  // Clear the previous geometry
  m_vertices.clear();
  m_bounds = frect();

  // No text: nothing to draw
  if (m_string.empty())
    return;

  // Precompute the variables needed by the algorithm
  float whitespaceWidth = m_font->getGlyph(L' ').advance;
  float x = 0.f;
  float y = 0.f;

  // Create one quad for each character
  float maxX = 0.f;
  float maxY = 0.f;
  float maxLineY = 0.f;
  std::uint32_t prevChar = 0;

  struct CharInfo {
    std::uint32_t chr;
    glm::vec2 pos;
    Color color;
    const Glyph *pGlyph{nullptr};
  };

  // reset to default color
  auto color = m_color;
  auto lastWordIndexSaved = -1;

  int lastWordIndex = 0;
  std::vector<CharInfo> charInfos(m_string.size());
  auto text = m_string;
  for (auto i = 0; i < static_cast<int>(text.size()); ++i) {
    wchar_t curChar = text[i];
    charInfos[i].chr = curChar;
    charInfos[i].pos = {x, y};
    charInfos[i].color = color;

    // Skip the \r char to avoid weird graphical issues
    if (curChar == '\r')
      continue;

    // Apply the kerning offset
    x += m_font->getKerning(prevChar, curChar, m_characterSize);

    prevChar = curChar;

    // Handle special characters
    if ((curChar == L' ') || (curChar == L'\n') || (curChar == L'\t') ||
        (curChar == L'#')) {
      if (m_maxWidth && x >= m_maxWidth) {
        y += maxLineY;
        // maxLineY = 0;
        x = 0;
        if (lastWordIndexSaved != lastWordIndex) {
          i = lastWordIndex;
          lastWordIndexSaved = lastWordIndex;
        }
        continue;
      }

      switch (curChar) {
      case L' ':x += whitespaceWidth;
        lastWordIndex = i;
        break;
      case L'\t':x += whitespaceWidth * 2;
        lastWordIndex = i;
        break;
      case L'\n':y += maxLineY;
        lastWordIndex = i;
        x = 0;
        // maxLineY = 0;
        break;
      case L'#':auto strColor = m_string.substr(i + 1, 6);
        auto str = StringHelper::tostring(strColor);
        color = Color::parseHex6(str.c_str());
        i += 6;
        break;
      }

      // Next glyph, no need to create a quad for whitespace
      continue;
    }

    // Extract the current glyph's description
    const auto *pGlyph = &m_font->getGlyph(curChar);
    charInfos[i].pGlyph = pGlyph;
    maxLineY = std::max(maxLineY, pGlyph->bounds.getHeight());

    // Advance to the next character
    x += pGlyph->advance;
  }

  maxLineY = 0.f;
  for (auto i = 0; i < static_cast<int>(m_string.size()); ++i) {
    auto info = charInfos[i];

    // Skip the \r char to avoid weird graphical issues
    if (info.chr == '\r')
      continue;

    // Handle special characters
    if ((info.chr == L' ') || (info.chr == L'\n') || (info.chr == L'\t') ||
        (info.chr == L'#')) {
      switch (info.chr) {
      case L' ':
      case L'\t': break;
      case L'\n': maxLineY = 0.f;
        break;
      case L'#': i += 6;
        break;
      }

      // Next glyph, no need to create a quad for whitespace
      continue;
    }

    // Add the glyph to the vertices
    addGlyphQuad(m_vertices, info.pos, info.color, *charInfos[i].pGlyph, *m_fontTexture);

    // Update the current bounds with the non outlined glyph bounds
    maxX = std::max(maxX, info.pos.x + charInfos[i].pGlyph->bounds.getWidth());
    maxY = std::max(maxY, info.pos.y + charInfos[i].pGlyph->bounds.getHeight());
    maxLineY = std::max(maxLineY, charInfos[i].pGlyph->bounds.getHeight());
  }

  maxY += maxLineY;

  // Update the bounding rectangle
  m_bounds.min = glm::vec2();
  m_bounds.max = {maxX, maxY};
}
}
