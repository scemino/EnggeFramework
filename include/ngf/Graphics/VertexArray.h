#pragma once

namespace ngf {
class VertexArray {
public:
  VertexArray();
  ~VertexArray();

  static void bind(const VertexArray* pVertexArray);

private:
  unsigned int m_vao{0};
};
}
