#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Text.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/Math/VectorExtensions.h>
#include <ngf/System/StringHelper.h>
#include <utility>
#include <sstream>

namespace ngf {
namespace {

struct ParagraphLine {
  std::vector<std::wstring> words;
  float indent = 0.0f;
  float spacing = 0.0f;
};

struct Paragraph {
  std::vector<ParagraphLine> lines;
};

std::vector<std::wstring> splitInParagraphs(std::wstring_view str) {
  std::vector<std::wstring> result;
  auto ss = std::wstringstream(str.data());

  for (std::wstring line; std::getline(ss, line, L'\n');)
    result.push_back(line);

  return result;
}

std::vector<std::wstring> splitInWords(std::wstring_view str) {
  std::vector<std::wstring> result;
  auto ss = std::wstringstream(str.data());

  for (std::wstring line; std::getline(ss, line, L' ');)
    result.push_back(line);

  return result;
}

float getWordWidth(std::wstring_view word, unsigned characterSize, const Font &font) {
  assert(!word.empty());

  float width = 0.0f;
  char32_t prevCodepoint = '\0';

  for (char32_t currCodepoint : word) {
    width += font.getKerning(prevCodepoint, currCodepoint, characterSize);
    prevCodepoint = currCodepoint;

    const Glyph &glyph = font.getGlyph(currCodepoint, characterSize);
    width += glyph.advance;
  }

  return width;
}

std::vector<Paragraph> makeParagraphs(const std::wstring &str,
                                      float spaceWidth,
                                      float paragraphWidth,
                                      Alignment align,
                                      unsigned characterSize,
                                      const Font &font) {
  auto paragraphs = splitInParagraphs(str);
  std::vector<Paragraph> out;

  for (const auto &simpleParagraph : paragraphs) {
    auto words = splitInWords(simpleParagraph);

    Paragraph paragraph;

    if (align == Alignment::None) {
      ParagraphLine line;
      line.words = std::move(words);
      line.indent = 0.0f;
      line.spacing = spaceWidth;
      paragraph.lines.push_back(std::move(line));
    } else {
      ParagraphLine currentLine;
      float currentWidth = 0.0f;

      for (const auto &word : words) {
        auto wordWith = getWordWidth(word, characterSize, font);

        if (!currentLine.words.empty() && currentWidth + spaceWidth + wordWith > paragraphWidth) {
          auto wordCount = currentLine.words.size();

          switch (align) {
          case Alignment::Left:currentLine.indent = 0.0f;
            currentLine.spacing = spaceWidth;
            break;

          case Alignment::Right:currentLine.indent = paragraphWidth - currentWidth;
            currentLine.spacing = spaceWidth;
            break;

          case Alignment::Center:currentLine.indent = (paragraphWidth - currentWidth) / 2;
            currentLine.spacing = spaceWidth;
            break;

          case Alignment::Justify:currentLine.indent = 0.0f;

            if (wordCount > 1) {
              currentLine.spacing = spaceWidth + (paragraphWidth - currentWidth) / (wordCount - 1);
            } else {
              currentLine.spacing = 0.0f;
            }

            break;

          case Alignment::None:assert(false);
            break;
          }

          paragraph.lines.push_back(std::move(currentLine));
          currentLine.words.clear();
        }

        if (currentLine.words.empty()) {
          currentWidth = wordWith;
        } else {
          currentWidth += spaceWidth + wordWith;
        }

        currentLine.words.push_back(word);
      }

      // add the last line
      if (!currentLine.words.empty()) {
        switch (align) {
        case Alignment::Left:
        case Alignment::Justify:currentLine.indent = 0.0f;
          currentLine.spacing = spaceWidth;
          break;

        case Alignment::Right:currentLine.indent = paragraphWidth - currentWidth;
          currentLine.spacing = spaceWidth;
          break;

        case Alignment::Center:currentLine.indent = (paragraphWidth - currentWidth) / 2;
          currentLine.spacing = spaceWidth;
          break;

        case Alignment::None:assert(false);
          break;
        }

        paragraph.lines.push_back(std::move(currentLine));
      }
    }

    out.push_back(std::move(paragraph));
    paragraph.lines.clear();
  }

  return out;
}

glm::vec2 normalize(const Texture &texture, const glm::ivec2 &v) {
  auto textureSize = glm::vec(texture.getSize());
  return glm::vec2(static_cast<float>(v.x) / textureSize.x,
                   static_cast<float>(v.y) / textureSize.y);
}

void addGlyphVertex(std::vector<Vertex> &array, const Texture &texture, const Glyph &glyph, const glm::vec2 &position) {
  Vertex vertices[4];

  vertices[0].pos = position + glyph.bounds.getTopLeft();
  vertices[1].pos = position + glyph.bounds.getTopRight();
  vertices[2].pos = position + glyph.bounds.getBottomLeft();
  vertices[3].pos = position + glyph.bounds.getBottomRight();

  vertices[0].texCoords = normalize(texture, glyph.textureRect.getTopLeft());
  vertices[1].texCoords = normalize(texture, glyph.textureRect.getTopRight());
  vertices[2].texCoords = normalize(texture, glyph.textureRect.getBottomLeft());
  vertices[3].texCoords = normalize(texture, glyph.textureRect.getBottomRight());

  // first triangle
  array.push_back(vertices[0]);
  array.push_back(vertices[1]);
  array.push_back(vertices[2]);

  // second triangle
  array.push_back(vertices[2]);
  array.push_back(vertices[1]);
  array.push_back(vertices[3]);
}

} // anonymous namespace

Text::Text() = default;

Text::Text(std::wstring string, const Font &font, unsigned int characterSize)
    : m_string(std::move(string)), m_font(&font),
      m_characterSize(characterSize) {
  ensureGeometryUpdate();
}

void Text::setWideString(const std::wstring &string) {
  if (m_string == string)
    return;
  m_string = string;
  ensureGeometryUpdate();
}

void Text::setString(const std::string &string) {
  setWideString(StringHelper::towstring(string));
}

std::string Text::getString() const {
  return StringHelper::tostring(m_string);
}

void Text::setFont(const Font &font) {
  if (m_font == &font)
    return;
  m_font = &font;
  ensureGeometryUpdate();
}

void Text::setMaxWidth(float maxWidth) {
  if (m_maxWidth == maxWidth)
    return;
  m_maxWidth = maxWidth;
  ensureGeometryUpdate();
}

void Text::setColor(const Color &color) {
  if (m_color == color)
    return;
  m_color = color;
  for (auto &vertex : m_vertices) {
    vertex.color = color;
  }
}

const Font *Text::getFont() const {
  return m_font;
}

frect Text::getLocalBounds() const {
  return m_bounds;
}

void Text::setAnchor(Anchor anchor) {
  m_transform.setOriginFromAnchorAndBounds(anchor, getLocalBounds());
}

void Text::draw(RenderTarget &target, RenderStates states) const {
  if (!m_font)
    return;

  RenderStates s = states;
  s.texture = m_fontTexture;
  s.transform *= m_transform.getTransform();

  if (m_outlineThickness > 0) {
    target.draw(PrimitiveType::Triangles, m_outlineVertices, s);
  }

  target.draw(PrimitiveType::Triangles, m_vertices, s);
}

void Text::ensureGeometryUpdate() {
  if (!m_font || m_string.empty())
    return;

  // Save the current fonts texture id
  m_fontTexture = &m_font->getTexture(m_characterSize);

  m_vertices.clear();
  m_outlineVertices.clear();

  m_bounds = frect();

  auto spaceWidth = m_font->getGlyph(' ', m_characterSize).advance;
  auto additionalSpace = (spaceWidth / 3.0f) * (m_letterSpacingFactor - 1.0f); // same as SFML even if weird
  spaceWidth += additionalSpace;
  auto lineHeight = m_font->getLineSpacing(m_characterSize) * m_lineSpacingFactor;

  auto paragraphs = makeParagraphs(m_string, spaceWidth, m_maxWidth, m_align, m_characterSize, *m_font);

  glm::vec2 position(0.0f, 0.0f);
  glm::vec2 min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
  glm::vec2 max(0.0f, 0.0f);

  for (const auto &paragraph : paragraphs) {
    for (const auto &line : paragraph.lines) {
      position.x = line.indent;

      for (const auto &word : line.words) {
        wchar_t prevCodepoint = '\0';

        for (wchar_t currCodepoint : word) {
          position.x += m_font->getKerning(prevCodepoint, currCodepoint, m_characterSize);
          prevCodepoint = currCodepoint;

          if (m_outlineThickness > 0.0f) {
            const auto &glyph = m_font->getGlyph(currCodepoint, m_characterSize, m_outlineThickness);
            addGlyphVertex(m_outlineVertices, *m_fontTexture, glyph, position);
            min = ngf::min(min, position + glyph.bounds.getTopLeft());
            max = ngf::max(max, position + glyph.bounds.getBottomRight());
          }

          const auto &glyph = m_font->getGlyph(currCodepoint, m_characterSize);
          addGlyphVertex(m_vertices, *m_fontTexture, glyph, position);

          if (m_outlineThickness == 0.0f) {
            min = ngf::min(min, position + glyph.bounds.getTopLeft());
            max = ngf::max(max, position + glyph.bounds.getBottomRight());
          }

          position.x += glyph.advance + additionalSpace;
        }
        position.x += line.spacing;
      }
      position.y += lineHeight;
    }
  }

  m_bounds = frect::fromMinMax(min, max);

  if (m_align != Alignment::None && m_maxWidth > 0.0f) {
    m_bounds.min.x = 0;
    m_bounds.max.x = m_maxWidth;
  }

  // update colors
  for (auto &vertex : m_vertices) {
    vertex.color = m_color;
  }
  for (auto &vertex : m_outlineVertices) {
    vertex.color = m_outlineColor;
  }
}

void Text::setOutlineThickness(float thickness) {
  m_outlineThickness = thickness;
  ensureGeometryUpdate();
}

float Text::getOutlineThickness() const {
  return m_outlineThickness;
}

void Text::setOutlineColor(const Color &color) {
  m_outlineColor = color;

  for (auto &vertex : m_outlineVertices) {
    vertex.color = color;
  }
}
Color Text::getOutlineColor() const {
  return m_outlineColor;
}

void Text::setAlignment(Alignment align) {
  if (m_align == align)
    return;

  m_align = align;
  ensureGeometryUpdate();
}

void Text::setCharacterSize(unsigned int characterSize) {
  if (m_characterSize == characterSize)
    return;
  m_characterSize = characterSize;
  ensureGeometryUpdate();
}

void Text::setLineSpacing(float spacingFactor) {
  if (m_lineSpacingFactor == spacingFactor)
    return;
  m_lineSpacingFactor = spacingFactor;
  ensureGeometryUpdate();
}

void Text::setLetterSpacing(float letterSpacingFactor) {
  if (m_letterSpacingFactor == letterSpacingFactor)
    return;
  m_letterSpacingFactor = letterSpacingFactor;
  ensureGeometryUpdate();
}
}
