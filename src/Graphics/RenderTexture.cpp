#include "RenderTexture.h"
#include "GlDebug.h"

namespace ngf {
RenderTexture::RenderTexture(glm::ivec2 size)
    : RenderTarget(size), m_texture(Texture::Format::Rgba, size, nullptr) {
  Texture::bind(nullptr);

  GL_CHECK(glGenFramebuffers(1, &m_handle));
  GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_handle));
  GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.getHandle(), 0));
  assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
  GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

RenderTexture::~RenderTexture() {
  GL_CHECK(glDeleteFramebuffers(1, &m_handle));
}

void RenderTexture::activate() {
  if (m_handle != 0) {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_handle));
  }
}

void RenderTexture::display() {
  GL_CHECK(glFlush());
}
}
