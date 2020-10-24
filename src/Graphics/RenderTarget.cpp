#include <ngf/Graphics/RenderTarget.h>
#include <Graphics/VertexArray.h>
#include <Graphics/Image.h>
#include <Graphics/Transform.h>
#include "GlDebug.h"

namespace ngf {

namespace {
Image createWhitePixel() {
  uint8_t pixel[] = {0xFF, 0xFF, 0xFF, 0xFF};
  return Image({1, 1}, pixel);
}

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

GLenum getEnum(BlendEquation equation) {
  switch (equation) {
  case BlendEquation::Add:return GL_FUNC_ADD;
  case BlendEquation::Subtract:return GL_FUNC_SUBTRACT;
  case BlendEquation::ReverseSubtract:return GL_FUNC_REVERSE_SUBTRACT;
  }
  assert(false);
}

GLenum getEnum(BlendFactor func) {
  switch (func) {
  case BlendFactor::Zero:return GL_ZERO;
  case BlendFactor::One:return GL_ONE;
  case BlendFactor::SrcColor:return GL_SRC_COLOR;
  case BlendFactor::OneMinusSrcColor:return GL_ONE_MINUS_SRC_COLOR;
  case BlendFactor::DstColor:return GL_DST_COLOR;
  case BlendFactor::OneMinusDstColor:return GL_ONE_MINUS_DST_COLOR;
  case BlendFactor::SrcAlpha:return GL_SRC_ALPHA;
  case BlendFactor::OneMinusSrcAlpha:return GL_ONE_MINUS_SRC_ALPHA;
  case BlendFactor::DstAlpha:return GL_DST_ALPHA;
  case BlendFactor::OneMinusDstAlpha:return GL_ONE_MINUS_DST_ALPHA;
  }
  assert(false);
}

constexpr size_t countPos = sizeof(glm::vec2) / sizeof(float);
constexpr size_t countColor = sizeof(Color) / sizeof(float);

constexpr std::array<VertexAttribute, 3> attributes
    {{
         {"a_position", countPos, VertexAttributeType::Float, false, offsetof(Vertex, pos)},
         {"a_color", countColor, VertexAttributeType::Float, false, offsetof(Vertex, color)},
         {"a_texCoords", countPos, VertexAttributeType::Float, false, offsetof(Vertex, texCoords)}
     }};

const char *vertexShaderSource =
    R"(#version 330 core
precision mediump float;
layout (location = 0) in vec2 a_position;
layout (location = 1) in vec4 a_color;
layout (location = 2) in vec2 a_texCoords;

uniform mat3 u_transform;

out vec4 v_color;
out vec2 v_texCoords;

void main(void) {
  v_color = a_color;
  v_texCoords = a_texCoords;
  vec3 worldPosition = vec3(a_position, 1);
  vec3 normalizedPosition = worldPosition * u_transform;
  gl_Position = vec4(normalizedPosition.xy, 0, 1);
})";

const char *fragmentShaderSource =
    R"(#version 330 core
precision mediump float;
out vec4 FragColor;
in vec4 v_color;
in vec2 v_texCoords;

uniform sampler2D u_texture;

void main(void) {
  vec4 texColor = texture(u_texture, v_texCoords);
  FragColor = v_color * texColor;
}
)";

void setBuffer(const Vertex *vertices, size_t sizeVertices, const std::uint16_t *indices, size_t sizeIndices) {
  ngf::VertexBuffer buf;
  constexpr size_t VertexSize = sizeof(Vertex);
  buf.buffer(ngf::VertexBuffer::Type::Array, sizeVertices * VertexSize, vertices);
  buf.buffer(ngf::VertexBuffer::Type::Element, sizeIndices * sizeof(std::uint16_t), indices);

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

RenderTarget::RenderTarget(glm::uvec2 size)
    : m_size(size), m_emptyTexture(createWhitePixel()) {
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
                        size_t sizeIndices,
                        const RenderStates &states) {

  ngf::VertexArray::bind(&m_vao);

  // set texture
  const Texture *pTexture = states.texture;
  if (!pTexture) {
    pTexture = &m_emptyTexture;
  }
  m_defaultShader.setUniform("u_texture", *pTexture);

  // set transform
  Transform viewTrsf;
  glm::vec2 size = m_size;
  glm::vec2 factors = 2.0f / size;
  viewTrsf.setScale(factors);

  auto transform = viewTrsf.getTransform() * states.transform;
  m_defaultShader.setUniform("u_transform", transform);

  // set blending
  GL_CHECK(glEnable(GL_BLEND));
  GL_CHECK(glBlendEquationSeparate(getEnum(states.mode.colorEquation), getEnum(states.mode.alphaEquation)));
  GL_CHECK(glBlendFuncSeparate(
      getEnum(states.mode.colorSrcFactor), getEnum(states.mode.colorDstFactor),
      getEnum(states.mode.alphaSrcFactor), getEnum(states.mode.alphaDstFactor)
  ));

  // set vertices and indices
  setBuffer(vertices, sizeVertices, indices, sizeIndices);

  ngf::Shader::bind(&m_defaultShader);
  ngf::VertexArray::bind(&m_vao);

  // draw
  GL_CHECK(glDrawElements(getEnum(primitiveType), sizeIndices, GL_UNSIGNED_SHORT, nullptr));

  GL_CHECK(glDisable(GL_BLEND));
  ngf::VertexArray::bind(nullptr);
  ngf::Shader::bind(nullptr);
}

}
