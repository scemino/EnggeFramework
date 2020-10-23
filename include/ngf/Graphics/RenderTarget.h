#pragma once
#include <ngf/Graphics/VertexBuffer.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/Graphics/VertexArray.h>
#include "RenderStates.h"

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

class RenderTarget {
public:
  RenderTarget();

  void clear(const Color &color);
  void draw(PrimitiveType primitiveType,
            const Vertex *vertices,
            size_t sizeVertices,
            const std::uint16_t *indices,
            size_t sizeIndices,
            const RenderStates& = {});

private:
  ngf::VertexArray m_vao;
  ngf::Shader m_defaultShader{};
  ngf::Texture m_emptyTexture{};
};
}
