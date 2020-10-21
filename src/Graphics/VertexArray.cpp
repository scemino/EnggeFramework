#include <ngf/Graphics/VertexArray.h>
#include <GL/glew.h>

namespace ngf {
VertexArray::VertexArray() {
  glGenVertexArrays(1, &m_vao);
}

VertexArray::~VertexArray() {
  glDeleteBuffers(1, &m_vao);
}

void VertexArray::bind() const {
  glBindVertexArray(m_vao);
}

void VertexArray::unbind() {
  glBindVertexArray(0);
}
}