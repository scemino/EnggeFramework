#include <ngf/Graphics/VertexBuffer.h>

namespace ngf {

namespace {
GLenum getTarget(VertexBuffer::Type type) {
  return type == VertexBuffer::Type::Array ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER;
}
}

VertexBuffer::VertexBuffer(Type type) : m_type(type) {
  glGenBuffers(1, &m_vbo);
}

VertexBuffer::~VertexBuffer() {
  glDeleteBuffers(1, &m_vbo);
}

void VertexBuffer::buffer(size_t size, const void *data) const {
  auto target = getTarget(m_type);
  glBindBuffer(target, m_vbo);
  glBufferData(target, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::bind() const {
  auto target = getTarget(m_type);
  glBindBuffer(target, m_vbo);
}

void VertexBuffer::unbind(Type type) {
  auto target = getTarget(type);
  glBindBuffer(target, 0);
}
}
