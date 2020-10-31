#include <ngf/Graphics/Texture.h>
#include <Graphics/Image.h>
#include <fstream>
#include "GlDebug.h"

namespace ngf {

namespace {
GLenum getGlType(Texture::Type type) {
  return type == Texture::Type::Texture2D ? GL_TEXTURE_2D : GL_TEXTURE_1D;
}

GLenum getGlFilter(bool smooth) {
  return smooth ? GL_LINEAR : GL_NEAREST;
}

GLenum getGlFormat(Texture::Format format) {
  switch (format) {
  case Texture::Format::Alpha: return GL_RED;
  case Texture::Format::Rgba: return GL_RGBA;
  case Texture::Format::Rgb: return GL_RGB;
  }
  assert(false);
}
}

Texture::Texture(Type type, Format format)
    : m_type(type), m_format{format} {
  GL_CHECK(glGenTextures(1, &m_img_tex));
}

Texture::Texture(const Image &image) {
  m_type = Type::Texture2D;
  m_format = image.getBpp() != 4 ? ngf::Texture::Format::Rgb : ngf::Texture::Format::Rgba;
  auto type = getGlType(m_type);
  GL_CHECK(glBindTexture(type, m_img_tex));
  auto glFormat = getGlFormat(m_format);
  m_size = image.getSize();
  glTexImage2D(GL_TEXTURE_2D,
               0,
               glFormat,
               image.getSize().x,
               image.getSize().y,
               0,
               glFormat,
               GL_UNSIGNED_BYTE,
               nullptr);
  loadFromMemory(image.getSize(), image.getPixelsPtr());
}

Texture::Texture(Format format, glm::uvec2 size, const void *data)
    : m_type(Type::Texture2D), m_format{format}, m_size{size} {
  GL_CHECK(glGenTextures(1, &m_img_tex));
  bind(this);
  auto glFormat = getGlFormat(format);
  glTexImage2D(GL_TEXTURE_2D, 0, glFormat, size.x, size.y, 0, glFormat, GL_UNSIGNED_BYTE, data);
  updateFilters();
}

Texture::Texture(Format format, const int width, const void *data)
    : m_type(Type::Texture1D), m_format{format}, m_size{width, 1} {
  GL_CHECK(glGenTextures(1, &m_img_tex));
  bind(this);
  auto glFormat = getGlFormat(format);
  glTexImage1D(GL_TEXTURE_1D, 0, glFormat, width, 0, glFormat, GL_UNSIGNED_BYTE, data);
  updateFilters();
}

Texture::Texture(const std::filesystem::path& path){
  GL_CHECK(glGenTextures(1, &m_img_tex));
  load(path);
}

Texture::~Texture() {
  GL_CHECK(glDeleteTextures(1, &m_img_tex));
}

void Texture::load(const std::filesystem::path& path) {
  std::ifstream is(path, std::ios::binary);
  loadFromStream(is);
}

void Texture::loadFromStream(std::istream& input){
  ngf::Image img;
  img.load(input);
  m_format = img.getBpp() == 4 ? ngf::Texture::Format::Rgba : ngf::Texture::Format::Rgb;
  auto type = getGlType(m_type);
  GL_CHECK(glBindTexture(type, m_img_tex));
  auto glFormat = getGlFormat(m_format);
  glTexImage2D(GL_TEXTURE_2D, 0, glFormat, img.getSize().x, img.getSize().y, 0, glFormat, GL_UNSIGNED_BYTE, nullptr);
  loadFromMemory(img.getSize(), img.getPixelsPtr());
}

void Texture::setSmooth(bool smooth) {
  if (m_smooth == smooth) {
    return;
  }

  m_smooth = smooth;

  if (!m_img_tex)
    return;

  GL_CHECK(glBindTexture(getGlType(m_type), m_img_tex));
  updateFilters();
}

bool Texture::isSmooth() const noexcept {
  return m_smooth;
}

void Texture::loadFromMemory(glm::uvec2 size, const void *data) {
  auto type = getGlType(m_type);
  m_size = size;
  GL_CHECK(glBindTexture(type, m_img_tex));
  if (m_type == Type::Texture2D) {
    GL_CHECK(glTexSubImage2D(type, 0, 0, 0, size.x, size.y, getGlFormat(m_format), GL_UNSIGNED_BYTE, data));
  } else {
    GL_CHECK(glTexSubImage1D(type, 0, 0, size.x, getGlFormat(m_format), GL_UNSIGNED_BYTE, data));
  }
  GL_CHECK(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST));
  GL_CHECK(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, getGlFilter(m_smooth)));
}

void Texture::bind(const Texture *pTexture) {
  if (pTexture) {
    auto type = getGlType(pTexture->m_type);
    GL_CHECK(glBindTexture(type, pTexture->m_img_tex));
  } else {
    GL_CHECK(glBindTexture(GL_TEXTURE_1D, 0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
  }
}

void Texture::updateFilters() {
  auto type = getGlType(m_type);
  GL_CHECK(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, getGlFilter(m_smooth)));
  GL_CHECK(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, getGlFilter(m_smooth)));
}
}
