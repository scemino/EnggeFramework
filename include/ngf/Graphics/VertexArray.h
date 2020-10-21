#pragma once

namespace ngf {
class VertexArray {
public:
  VertexArray();
  ~VertexArray();

  void bind() const;
  static void unbind();

private:
  unsigned int m_vao{0};
};
}
