#include <ngf/Graphics/DefaultFont.h>
#include <cstring>
#include <stdexcept>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H
#include "GlDebug.h"

namespace ngf {
static constexpr float Scale = (1 << 6);
static constexpr int DefaultSize = 1024;
static constexpr int Padding = 1;

namespace {

float convert(FT_Pos value) {
  return static_cast<float>(value) / Scale;
}

uint64_t makeKey(char32_t codepoint, float thickness) {
  static_assert(sizeof(float) == sizeof(uint32_t), "Float is not 32 bits.");
  uint32_t hex;
  std::memcpy(&hex, &thickness, sizeof(float));
  uint64_t key = codepoint | static_cast<uint64_t>(hex) << 32;
  return key;
}

const char *FT_ErrorMessage(FT_Error error) {
  switch (error) {

#undef FTERRORS_H_
#define FT_ERRORDEF(e, v, s)  \
        case v:                       \
          return s;
#include FT_ERRORS_H

  default:break;
  };

  return "unknown error";
}

unsigned long callbackRead(FT_Stream rec, unsigned long offset, unsigned char *buffer, unsigned long count) {
  auto *stream = static_cast<std::istream *>(rec->descriptor.pointer);
  stream->seekg(offset, std::ios::beg);

  if (count == 0)
    return 0;

  stream->read((char *) buffer, count);
  return count;
}

void callbackClose(FT_Stream) {
}

} // anonymous namespace

DefaultFont::DefaultFont() {
  FT_Library library;

  if (auto err = FT_Init_FreeType(&library)) {
    throw std::runtime_error("Could not init Freetype library");
  }

  m_library = library;

  FT_Stroker stroker;

  if (auto err = FT_Stroker_New(library, &stroker)) {
    throw std::runtime_error("Could not create the stroker");
  }

  m_stroker = stroker;
}

DefaultFont::DefaultFont(const std::filesystem::path &filename)
    : DefaultFont() {
  auto library = static_cast<FT_Library>(m_library);
  // load face
  FT_Face face = nullptr;
  if (auto err = FT_New_Face(library, filename.string().c_str(), 0, &face)) {
    throw std::runtime_error("Could not create the font face");
  }
  m_face = face;
}

DefaultFont::DefaultFont(std::istream &stream)
    : DefaultFont() {
  auto library = static_cast<FT_Library>(m_library);

  // load face
  FT_StreamRec rec;
  std::memset(&rec, 0, sizeof(FT_StreamRec));
  rec.base = nullptr;
  rec.size = 0x7FFFFFFF; // unknown size
  rec.pos = 0;
  rec.descriptor.pointer = &stream;
  rec.read = callbackRead;
  rec.close = callbackClose;

  FT_Open_Args args;
  std::memset(&args, 0, sizeof(FT_Open_Args));
  args.flags = FT_OPEN_STREAM;
  args.stream = &rec;
  args.driver = nullptr;

  FT_Face face = nullptr;
  if (auto err = FT_Open_Face(library, &args, 0, &face)) {
    throw std::runtime_error("Could not create the font face from stream");
  }

  m_face = face;
}

DefaultFont::DefaultFont(uint8_t *content, size_t size)
    : DefaultFont() {
  auto library = static_cast<FT_Library>(m_library);

  // load face
  FT_Face face = nullptr;
  if (auto
      err = FT_New_Memory_Face(library, static_cast<const FT_Byte *>(content), size, 0, &face)) {
    throw std::runtime_error("Could not create the font face");
  }
  m_face = face;
}

DefaultFont::~DefaultFont() {
  if (m_face != nullptr) {
    if (auto err = FT_Done_Face(static_cast<FT_Face>(m_face))) {
      std::cerr << "Could not destroy the font face: " << FT_ErrorMessage(err) << '\n';
    }
  }

  if (m_stroker != nullptr) {
    // no possible error for this call
    FT_Stroker_Done(static_cast<FT_Stroker>(m_stroker));
  }

  if (m_library != nullptr) {
    if (auto err = FT_Done_FreeType(static_cast<FT_Library>(m_library))) {
      std::cerr << "Could not destroy Freetype library: " << FT_ErrorMessage(err) << '\n';
    }
  }
}

DefaultFont::DefaultFont(DefaultFont &&other) noexcept
    : m_library(std::exchange(other.m_library, nullptr)), m_stroker(std::exchange(other.m_stroker, nullptr)),
      m_face(std::exchange(other.m_face, nullptr)), m_currentCharacterSize(other.m_currentCharacterSize),
      m_cache(std::move(other.m_cache)) {
}

void DefaultFont::loadFromFile(const std::filesystem::path &path) {
  auto library = static_cast<FT_Library>(m_library);
  // load face
  FT_Face face = nullptr;
  if (auto err = FT_New_Face(library, path.string().c_str(), 0, &face)) {
    throw std::runtime_error("Could not create the font face");
  }
  m_face = face;
}

DefaultFont &DefaultFont::operator=(DefaultFont &&other) noexcept {
  std::swap(m_library, other.m_library);
  std::swap(m_stroker, other.m_stroker);
  std::swap(m_face, other.m_face);
  std::swap(m_cache, other.m_cache);
  return *this;
}

const Glyph &DefaultFont::getGlyph(unsigned int codepoint, unsigned int characterSize, float outlineThickness) {
  auto cacheIt = m_cache.find(characterSize);

  if (cacheIt == m_cache.end()) {
    GlyphCache cache = createCache(characterSize);
    std::tie(cacheIt, std::ignore) = m_cache.insert(std::make_pair(characterSize, std::move(cache)));
  }

  auto &cache = cacheIt->second;

  auto key = makeKey(codepoint, outlineThickness);
  auto glyphIt = cache.glyphs.find(key);

  if (glyphIt == cache.glyphs.end()) {
    auto glyph = createGlyph(codepoint, characterSize, outlineThickness, cache);
    std::tie(glyphIt, std::ignore) = cache.glyphs.insert(std::make_pair(key, glyph));
  }

  return glyphIt->second;
}

float DefaultFont::getKerning(unsigned int left, unsigned int right, unsigned int characterSize) {
  if (left == 0 || right == 0)
    return 0.0f;

  if (m_face == nullptr)
    return 0.0f;

  if (!setCurrentCharacterSize(characterSize))
    return 0.0f;

  auto face = static_cast<FT_Face>(m_face);

  if (!FT_HAS_KERNING(face)) {
    return 0.0f;
  }

  auto indexLeft = FT_Get_Char_Index(face, left);
  auto indexRight = FT_Get_Char_Index(face, right);

  FT_Vector kerning;
  if (auto err = FT_Get_Kerning(face, indexLeft, indexRight, FT_KERNING_UNFITTED, &kerning)) {
    std::cerr << "Could not get kerning: " << FT_ErrorMessage(err) << '\n';
  }

  return convert(kerning.x);
}

float DefaultFont::getLineSpacing(unsigned characterSize) {
  if (m_face == nullptr)
    return 0.0f;

  if (!setCurrentCharacterSize(characterSize))
    return 0.0f;

  auto face = static_cast<FT_Face>(m_face);
  return convert(face->size->metrics.height);
}

const Texture *DefaultFont::getTexture(unsigned int characterSize) {
  auto it = m_cache.find(characterSize);
  if (it == m_cache.end())
    return nullptr;

  GlyphCache &cache = it->second;
  return cache.texture.get();
}

void DefaultFont::generateTexture(unsigned int characterSize) {
  getGlyph(' ', characterSize, 0);
}

DefaultFont::GlyphCache DefaultFont::createCache(unsigned int characterSize) {
  GlyphCache cache;

  auto texture = std::make_unique<Texture>(ngf::Texture::Format::Alpha, glm::ivec2{DefaultSize, DefaultSize}, nullptr);
  cache.texture = std::move(texture);

  // create the glyphs for the usual characters
  for (char c : "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") {
    auto glyph = createGlyph(c, characterSize, 0.0f, cache);
    auto key = makeKey(c, 0.0f);
    cache.glyphs.insert(std::make_pair(key, glyph));
  }

  return cache;
}

Glyph DefaultFont::createGlyph(unsigned int codepoint,
                               unsigned int characterSize,
                               float outlineThickness,
                               GlyphCache &cache) {
  Glyph out;

  if (m_face == nullptr)
    return out;

  if (!setCurrentCharacterSize(characterSize))
    return out;

  auto face = static_cast<FT_Face>(m_face);

  FT_Int32 flags = FT_LOAD_TARGET_NORMAL | FT_LOAD_FORCE_AUTOHINT;

  if (outlineThickness > 0) {
    flags |= FT_LOAD_NO_BITMAP;
  }

  if (auto err = FT_Load_Char(face, codepoint, flags)) {
    std::cerr << "Could not load the glyph: " << FT_ErrorMessage(err) << '\n';
    return out;
  }

  auto slot = face->glyph;
  FT_Glyph glyph;
  if (auto err = FT_Get_Glyph(slot, &glyph)) {
    std::cerr << "Could not extract the glyph: " << FT_ErrorMessage(err) << '\n';
    return out;
  }

  if (outlineThickness > 0) {
    assert(glyph->format == FT_GLYPH_FORMAT_OUTLINE);

    auto stroker = static_cast<FT_Stroker>(m_stroker);

    FT_Stroker_Set(stroker,
                   static_cast<FT_Fixed>(outlineThickness * Scale),
                   FT_STROKER_LINECAP_ROUND,
                   FT_STROKER_LINEJOIN_ROUND,
                   0);
    FT_Glyph_Stroke(&glyph, stroker, 0);
  }

  if (auto err = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, 1)) {
    std::cerr << "Could create a bitmap from the glyph: " << FT_ErrorMessage(err) << '\n';
    FT_Done_Glyph(glyph);
    return out;
  }

  assert(glyph->format == FT_GLYPH_FORMAT_BITMAP);
  auto bglyph = reinterpret_cast<FT_BitmapGlyph>(glyph);

  // advance
  out.advance = convert(slot->metrics.horiAdvance);

  // size
  glm::ivec2 glyphSize(bglyph->bitmap.width, bglyph->bitmap.rows);

  if (glyphSize.y == 0 || glyphSize.x == 0) {
    FT_Done_Glyph(glyph);
    return out;
  }

  // textureRect (dumb online 2D bin packing)
  glyphSize += glm::vec2(2 * Padding, 2 * Padding);

  auto textureSize = cache.texture->getSize();

  if (cache.packing.right + glyphSize.x > textureSize.x) {
    cache.packing.right = 0;
    cache.packing.top = cache.packing.bottom;
  }

  if (cache.packing.top + glyphSize.y > textureSize.y) {
    // TODO: Houston, we have a problem!
    std::cerr << "Could not add a new glyph to the cache\n";
    FT_Done_Glyph(glyph);
    return out;
  }

  auto rect = irect::fromPositionSize({cache.packing.right, cache.packing.top}, glyphSize);
  if (rect.max.y > cache.packing.bottom) {
    cache.packing.bottom = rect.max.y;
  }

  cache.packing.right += (rect.max.x - rect.min.x);

  out.textureRect = cache.texture->computeTextureCoords(rect.shrink(Padding));

  // bounds
  if (outlineThickness == 0.0f) {
    out.bounds = frect::fromPositionSize({convert(slot->metrics.horiBearingX), -convert(slot->metrics.horiBearingY)},
                                         {convert(slot->metrics.width), convert(slot->metrics.height)});
  } else {
    out.bounds = frect::fromPositionSize({static_cast<float>(bglyph->left), -static_cast<float>(bglyph->top)},
                                         {static_cast<float>(bglyph->bitmap.width),
                                          static_cast<float>(bglyph->bitmap.rows)});
  }

  // bitmap
  auto size = rect.getSize();

  std::vector<uint8_t> paddedBuffer(static_cast<std::size_t>(size.x) * static_cast<std::size_t>(size.y), 0);
  const uint8_t *sourceBuffer = bglyph->bitmap.buffer;

  for (auto y = Padding; y < size.y - Padding; ++y) {
    for (auto x = Padding; x < size.x - Padding; ++x) {
      auto index = y * size.x + x;
      paddedBuffer[index] = sourceBuffer[x - Padding];
    }

    sourceBuffer += bglyph->bitmap.pitch;
  }

  cache.texture->update(paddedBuffer.data(), rect);

  GL_CHECK(glFlush());

  FT_Done_Glyph(glyph);
  return out;
}

bool DefaultFont::setCurrentCharacterSize(unsigned int characterSize) {
  if (m_currentCharacterSize == characterSize)
    return true;

  auto face = static_cast<FT_Face>(m_face);
  if (auto err = FT_Set_Pixel_Sizes(face, 0, characterSize)) {
    std::cerr << "Could not change the font size: " << FT_ErrorMessage(err) << '\n';
    return false;
  }

  m_currentCharacterSize = characterSize;
  return true;
}
}
