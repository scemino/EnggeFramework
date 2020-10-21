#include <ngf/Graphics/RenderTarget.h>
#include "GlDebug.h"

namespace ngf {

namespace {
GLenum getEnum(PrimitiveType type) {
  switch (type) {
  case PrimitiveType::Points:return GL_POINTS;
  case PrimitiveType::LineStrip:return GL_LINE_STRIP;
  case PrimitiveType::LineLoop:return GL_LINE_LOOP;
  case PrimitiveType::Lines:return GL_LINES;
  case PrimitiveType::TriangleStrip:return GL_TRIANGLE_STRIP;
  case PrimitiveType::TriangleFan:return GL_TRIANGLE_FAN;
  case PrimitiveType::Triangles:return GL_TRIANGLES;
  }
  assert(false);
}
}

void RenderTarget::draw(const PrimitiveType primitiveType,
                        const ElementType elementType,
                        size_t size,
                        const Shader *pShader) {
  if (pShader)
    Shader::bind(pShader);
  GL_CHECK(glDrawElements(getEnum(primitiveType), size, static_cast<GLenum>(elementType), nullptr));
}
}
