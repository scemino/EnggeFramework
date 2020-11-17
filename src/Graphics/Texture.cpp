#include <ngf/Graphics/Texture.h>
#include <ngf/Graphics/Image.h>
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
  return GL_RGBA;
}

Texture::Format toFormat(int bpp) {
  switch (bpp) {
  case 1 : return Texture::Format::Alpha;
  case 4: return Texture::Format::Rgba;
  case 3: return Texture::Format::Rgb;
  }
  assert(false);
  return Texture::Format::Rgba;
}

int getAlignment(Texture::Format format) {
  switch (format) {
  case Texture::Format::Rgba:return 4;
  case Texture::Format::Rgb:return 3;
  case Texture::Format::Alpha:return 1;
  }
  assert(false);
  return 4;
}
}

Texture::Texture(Type type, Format format)
    : m_type(type), m_format{format} {
  GL_CHECK(glGenTextures(1, &m_img_tex));
}

Texture::Texture(const Image &image)
    : Texture(toFormat(image.getBpp()), image.getSize(), image.getPixelsPtr()) {
}

Texture::Texture(Format format, glm::ivec2 size, const void *data)
    : m_type(Type::Texture2D), m_format(format), m_size(size) {
  auto glFormat = getGlFormat(format);
  auto glInternalFormat = format == Format::Alpha ? GL_R8 : glFormat;
  GL_CHECK(glGenTextures(1, &m_img_tex));
  GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_img_tex));
  GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, getAlignment(m_format)));
  GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, glInternalFormat, m_size.x, m_size.y, 0, glFormat, GL_UNSIGNED_BYTE, data));
  updateFilters();
}

Texture::Texture(Format format, const int width, const void *data)
    : m_type(Type::Texture1D), m_format{format}, m_size{width, 1} {
  GL_CHECK(glGenTextures(1, &m_img_tex));
  bind(this);
  auto glFormat = getGlFormat(format);
  GL_CHECK(glTexImage1D(GL_TEXTURE_1D, 0, glFormat, width, 0, glFormat, GL_UNSIGNED_BYTE, data));
  updateFilters();
}

Texture::Texture(glm::ivec2 size)
    : Texture(Format::Rgba, size, nullptr) {
}

Texture::Texture(const std::filesystem::path &path) {
  GL_CHECK(glGenTextures(1, &m_img_tex));
  load(path);
}

Texture::~Texture() {
  GL_CHECK(glDeleteTextures(1, &m_img_tex));
}

void Texture::load(const std::filesystem::path &path) {
  std::ifstream is(path, std::ios::binary);
  loadFromStream(is);
}

void Texture::loadFromStream(std::istream &input) {
  ngf::Image img;
  img.load(input);
  m_format = img.getBpp() == 4 ? ngf::Texture::Format::Rgba : ngf::Texture::Format::Rgb;
  auto type = getGlType(m_type);
  GL_CHECK(glBindTexture(type, m_img_tex));
  auto glFormat = getGlFormat(m_format);
  GL_CHECK(glTexImage2D(GL_TEXTURE_2D,
                        0,
                        glFormat,
                        img.getSize().x,
                        img.getSize().y,
                        0,
                        glFormat,
                        GL_UNSIGNED_BYTE,
                        nullptr));
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

void Texture::loadFromMemory(glm::ivec2 size, const void *data) {
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
  if (pTexture && pTexture->m_img_tex) {
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

frect Texture::computeTextureCoords(const irect &rect) const {
  glm::vec2 size = m_size;
  return frect::fromMinMax(static_cast<glm::vec2>(rect.min) / size, static_cast<glm::vec2>(rect.max) / size);
}

void Texture::update(const uint8_t *data, const irect &rect) {
  assert(0 <= rect.min.x);
  assert(0 <= rect.min.y);
  assert(rect.max.x <= m_size.x);
  assert(rect.max.y <= m_size.y);

  if (!m_img_tex || !data)
    return;

  m_mipmap = false;

  GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_img_tex));
  GL_CHECK(glTexSubImage2D(GL_TEXTURE_2D,
                           0,
                           rect.min.x,
                           rect.min.y,
                           rect.getWidth(),
                           rect.getHeight(),
                           GL_RED,
                           GL_UNSIGNED_BYTE,
                           data));

  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getGlFilter(m_smooth)));
  GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getGlFilter(m_smooth)));
  if (m_format == Format::Alpha) {
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED));
  }
}

Image Texture::copyToImage() const {
  if (!m_img_tex)
    return Image();

  auto size = getSize();
  std::vector<uint8_t> pixels(static_cast<std::size_t>(size.x) * static_cast<std::size_t>(size.y) * 4);

  GLuint framebuffer;
  GL_CHECK(glGenFramebuffers(1, &framebuffer));

  GLint boundFramebuffer;
  GL_CHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFramebuffer));

  GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
  GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_img_tex, 0));
  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

  GL_CHECK(glPixelStorei(GL_PACK_ALIGNMENT, 4));
  GL_CHECK(glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]));

  GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, boundFramebuffer));
  GL_CHECK(glDeleteFramebuffers(1, &framebuffer));

  Image image(size, pixels.data());
  return image;
}
}
