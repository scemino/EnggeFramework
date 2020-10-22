#include <ngf/Graphics/VertexBuffer.h>
#include "GlDebug.h"

namespace ngf {

namespace {
GLenum getTarget(VertexBuffer::Type type) {
  return type == VertexBuffer::Type::Array ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
}
}

VertexBuffer::VertexBuffer() {
  GL_CHECK(glGenBuffers(2, m_buffers.data()));
}

VertexBuffer::~VertexBuffer() {
  GL_CHECK(glDeleteBuffers(2, m_buffers.data()));
}

void VertexBuffer::buffer(Type type, size_t size, const void *data) {
  auto target = getTarget(type);
  auto index = type == VertexBuffer::Type::Array ? 0 : 1;
  GL_CHECK(glBindBuffer(target, m_buffers[index]));
  GL_CHECK(glBufferData(target, size, nullptr, GL_STATIC_DRAW));
  GL_CHECK(glBufferSubData(target, 0, size, data));
  GL_CHECK(glBindBuffer(target, 0));
}

void VertexBuffer::bind(const VertexBuffer *pVertexBuffer) {
  if (pVertexBuffer) {
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, pVertexBuffer->m_buffers[0]));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pVertexBuffer->m_buffers[1]));
  } else {
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
  }
}
}
