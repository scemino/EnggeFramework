#include <ngf/Graphics/VertexArray.h>
#include <GL/glew.h>

namespace ngf {
VertexArray::VertexArray() {
  glGenVertexArrays(1, &m_vao);
}

VertexArray::~VertexArray() {
  glDeleteBuffers(1, &m_vao);
}

void VertexArray::bind(const VertexArray *pVertexArray) {
  if (pVertexArray) {
    glBindVertexArray(pVertexArray->m_vao);
  } else {
    glBindVertexArray(0);
  }
}
}