#pragma once
#include "VertexBuffer.h"
#include "Shader.h"

namespace ngf {
enum class PrimitiveType {
  Points,
  LineStrip,
  LineLoop,
  Lines,
  TriangleStrip,
  TriangleFan,
  Triangles,
};

enum class ElementType {
  Byte = GL_BYTE,
  UnsignedByte = GL_UNSIGNED_BYTE,
  Short = GL_SHORT,
  UnsignedShort = GL_UNSIGNED_SHORT,
  Int = GL_INT,
  UnsignedInt = GL_UNSIGNED_INT,
};

class RenderTarget {
public:
  void draw(const PrimitiveType primitiveType, const ElementType elementType, size_t size, const Shader *pShader);
};
}
