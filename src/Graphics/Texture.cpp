#include <ngf/Graphics/Texture.h>
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

Texture::Texture(Format format, const int width, const int height, const void *data)
    : m_type(Type::Texture2D), m_format{format} {
  GL_CHECK(glGenTextures(1, &m_img_tex));
  bind();
  auto glFormat = getGlFormat(format);
  glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, data);
  updateFilters();
}

Texture::Texture(Format format, const int width, const void *data)
    : m_type(Type::Texture1D), m_format{format} {
  GL_CHECK(glGenTextures(1, &m_img_tex));
  bind();
  auto glFormat = getGlFormat(format);
  glTexImage1D(GL_TEXTURE_1D, 0, glFormat, width, 0, glFormat, GL_UNSIGNED_BYTE, data);
  updateFilters();
}

Texture::~Texture() {
  GL_CHECK(glDeleteTextures(1, &m_img_tex));
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

void Texture::setData(const int width, const int height, const void *data) const {
  auto type = getGlType(m_type);
  GL_CHECK(glBindTexture(type, m_img_tex));
  if (m_type == Type::Texture2D) {
    GL_CHECK(glTexSubImage2D(type, 0, 0, 0, width, height, getGlFormat(m_format), GL_UNSIGNED_BYTE, data));
  } else {
    GL_CHECK(glTexSubImage1D(type, 0, 0, width, getGlFormat(m_format), GL_UNSIGNED_BYTE, data));
  }
  GL_CHECK(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, m_smooth ? GL_LINEAR : GL_NEAREST));
  GL_CHECK(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, getGlFilter(m_smooth)));
}

void Texture::bind() const {
  auto type = getGlType(m_type);
  GL_CHECK(glBindTexture(type, m_img_tex));
}

void Texture::updateFilters() {
  auto type = getGlType(m_type);
  GL_CHECK(glTexParameteri(type, GL_TEXTURE_MAG_FILTER, getGlFilter(m_smooth)));
  GL_CHECK(glTexParameteri(type, GL_TEXTURE_MIN_FILTER, getGlFilter(m_smooth)));
}
}
