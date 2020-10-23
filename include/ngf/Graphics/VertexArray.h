#pragma once

namespace ngf {
class VertexArray {
public:
  VertexArray();
  ~VertexArray();
  VertexArray(const VertexArray&) = delete;
  VertexArray& operator=(const VertexArray&) = delete;

  static void bind(const VertexArray* pVertexArray);

private:
  unsigned int m_vao{0};
};
}
