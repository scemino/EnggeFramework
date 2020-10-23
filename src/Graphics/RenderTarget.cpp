#include <ngf/Graphics/RenderTarget.h>
#include <Graphics/VertexArray.h>
#include "GlDebug.h"

namespace ngf {

namespace {
enum class VertexAttributeType {
  Byte = GL_BYTE,
  Short = GL_SHORT,
  Float = GL_FLOAT,
};

struct VertexAttribute {
  const char *name;
  int size;
  VertexAttributeType type;
  bool normalized;
  std::size_t offset;
};

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

constexpr std::array<VertexAttribute, 2> attributes{{
                                                        {"a_position", 2, VertexAttributeType::Float, false,
                                                         offsetof(Vertex, pos)},
                                                        {"a_color", 4, VertexAttributeType::Float, false,
                                                         offsetof(Vertex, color)}
                                                    }};

const char *vertexShaderSource =
    R"(#version 100
precision mediump float;
attribute vec2 a_position;
attribute vec4 a_color;
varying vec4 v_color;

void main(void) {
  v_color = a_color;
  gl_Position = vec4(a_position.xy, 0, 1);
})";

const char *fragmentShaderSource =
    R"(#version 100
precision mediump float;
varying vec4 v_color;

void main(void) {
  gl_FragColor = v_color;
}
)";

void setBuffer(const Vertex *vertices, size_t sizeVertices, const std::uint16_t *indices, size_t sizeIndices) {
  ngf::VertexBuffer buf;
  buf.buffer(ngf::VertexBuffer::Type::Array, sizeVertices * sizeof(Vertex), vertices);
  buf.buffer(ngf::VertexBuffer::Type::Element, sizeIndices * sizeof(Vertex), indices);

  auto loc = 0;
  for (auto info : attributes) {
    glEnableVertexAttribArray(loc);
    const void *pointer = reinterpret_cast<const void *>(info.offset);
    glVertexAttribPointer(loc,
                          info.size,
                          static_cast<GLenum>(info.type),
                          info.normalized ? GL_TRUE : GL_FALSE,
                          sizeof(Vertex),
                          pointer);
    loc++;
  }
  VertexArray::bind(nullptr);
}
}

RenderTarget::RenderTarget() {
  m_defaultShader.load(vertexShaderSource, fragmentShaderSource);
}

void RenderTarget::clear(const Color &color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void RenderTarget::draw(PrimitiveType primitiveType,
                        const Vertex *vertices,
                        size_t sizeVertices,
                        const std::uint16_t *indices,
                        size_t sizeIndices) {

  ngf::VertexArray::bind(&m_vao);
  setBuffer(vertices, sizeVertices, indices, sizeIndices);

  ngf::Shader::bind(&m_defaultShader);
  ngf::VertexArray::bind(&m_vao);

  GL_CHECK(glDrawElements(getEnum(primitiveType), sizeIndices, GL_UNSIGNED_SHORT, nullptr));

  ngf::VertexArray::bind(nullptr);
  ngf::Shader::bind(nullptr);
}

}
