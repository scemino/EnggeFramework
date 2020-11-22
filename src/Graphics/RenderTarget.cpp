#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Image.h>
#include <ngf/Math/Transform.h>
#include <ngf/System/Window.h>
#include <utility>
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

void setBuffer(const Vertex *vertices, size_t sizeVertices) {
  ngf::VertexBuffer buf;
  constexpr size_t VertexSize = sizeof(Vertex);
  buf.buffer(ngf::VertexBuffer::Type::Array, sizeVertices * VertexSize, vertices);

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

RenderTarget::RenderTarget(glm::ivec2 size)
    : m_view(frect::fromPositionSize({0.0f, 0.0f}, {static_cast<float>(size.x), static_cast<float>(size.y)})),
      m_emptyTexture(createWhitePixel()) {
  m_defaultShader.load(vertexShaderSource, fragmentShaderSource);
}

void RenderTarget::clear(const Color &color) {
  GL_CHECK(glClearColor(color.r, color.g, color.b, color.a));
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
}

void RenderTarget::draw(PrimitiveType primitiveType,
                        const Vertex *vertices,
                        size_t sizeVertices,
                        const std::uint16_t *indices,
                        size_t sizeIndices,
                        RenderStates states) {
  ngf::VertexArray::bind(&m_vao);

  // set texture
  auto pTexture = states.texture;
  if (!pTexture) {
    pTexture = &m_emptyTexture;
  }

  // set shader
  auto shader = states.shader;
  if (!shader) {
    shader = &m_defaultShader;
  }
  shader->setUniform("u_texture", *pTexture);

  // set transform
  auto transform = states.transform * getView().getTransform();
  shader->setUniform("u_transform", transform);

  // set blending
  GL_CHECK(glEnable(GL_BLEND));
  GL_CHECK(glBlendEquationSeparate(getEnum(states.mode.colorEquation), getEnum(states.mode.alphaEquation)));
  GL_CHECK(glBlendFuncSeparate(
      getEnum(states.mode.colorSrcFactor), getEnum(states.mode.colorDstFactor),
      getEnum(states.mode.alphaSrcFactor), getEnum(states.mode.alphaDstFactor)
  ));

  // set vertices and indices
  if (indices) {
    setBuffer(vertices, sizeVertices, indices, sizeIndices);
  } else {
    setBuffer(vertices, sizeVertices);
  }

  ngf::Shader::bind(shader);
  ngf::VertexArray::bind(&m_vao);

  // draw
  if (indices) {
    GL_CHECK(glDrawElements(getEnum(primitiveType), sizeIndices, GL_UNSIGNED_SHORT, nullptr));
  } else {
    GL_CHECK(glDrawArrays(getEnum(primitiveType), 0, sizeVertices));
  }

  GL_CHECK(glDisable(GL_BLEND));
  ngf::VertexArray::bind(nullptr);
  ngf::Shader::bind(nullptr);
}

void RenderTarget::draw(PrimitiveType primitiveType,
                        const Vertex *vertices,
                        size_t sizeVertices,
                        RenderStates states) {
  draw(primitiveType, vertices, sizeVertices, nullptr, 0, states);
}

void RenderTarget::setView(const View &view) {
  m_view = view;

  // set the GL viewport everytime a new view is defined
  auto viewport = getCanonicalViewport(getView());
  GL_CHECK(glViewport(viewport.min.x, viewport.min.y, viewport.getWidth(), viewport.getHeight()));
}

irect RenderTarget::getCanonicalViewport(const View &view) const {
  auto size = getSize();
  const frect &viewport = view.getViewport();
  glm::ivec2 rectPos =
      {static_cast<int>(viewport.min.x * size.x + 0.5f), static_cast<int>((1.0f - viewport.max.y) * size.y + 0.5f)};
  glm::ivec2 rectSize =
      {static_cast<int>(viewport.getWidth() * size.x + 0.5f), static_cast<int>(viewport.getHeight() * size.y + 0.5f)};
  return irect::fromPositionSize(rectPos, rectSize);
}

irect RenderTarget::getViewport(const View &view) const {
  auto region = getCanonicalViewport(view);
  auto size = getSize();
  return irect::fromPositionSize({region.min.x, size.y - (region.min.y + region.getHeight())},
                                 {region.getWidth(), region.getHeight()});
}

glm::vec2 RenderTarget::mapPixelToCoords(glm::ivec2 point, const View &view) const {
  auto viewport = getViewport(view);

  /* simulate inverse projection transform
   * i.e. compute normalized device coordinates from screen coordinates
   *
   * 0 +---------+      1 +---------+
   *   |         |        |         |
   *   |         | ===>   |         |
   *   |         |        |         |
   * h +---------+     -1 +---------+
   *   0         w       -1         1
   */
  glm::vec2 normalized;
  normalized.x = 2.0f * (point.x - viewport.min.x) / viewport.getWidth() - 1;
  normalized.y = 1 - 2.0f * (point.y - viewport.min.y) / viewport.getHeight();

  /* apply inverse view transform
   * i.e. compute world coordinates from normalized device coordinates
   */
  return transform(view.getInverseTransform(), normalized);
}

glm::vec2 RenderTarget::mapPixelToCoords(glm::ivec2 point) const {
  return mapPixelToCoords(point, getView());
}

glm::ivec2 RenderTarget::mapCoordsToPixel(glm::vec2 point, const View &view) const {
  irect viewport = getViewport(view);

  /* apply view transform
   * i.e. compute normalized device coordinates from world coordinates
   */
  auto normalized = transform(view.getTransform(), point);

  /* simulate projection transform
   * i.e. compute screen coordinates from normalized device coordinates
   *
   *  1 +---------+     0 +---------+
   *    |         |       |         |
   *    |         | ===>  |         |
   *    |         |       |         |
   * -1 +---------+     h +---------+
   *   -1         1       0         w
   */
  glm::ivec2 pixel;
  pixel.x = static_cast<int>((1 + normalized.x) / 2 * viewport.getWidth() + viewport.min.x);
  pixel.y = static_cast<int>((1 - normalized.y) / 2 * viewport.getHeight() + viewport.min.y);

  return pixel;
}

glm::ivec2 RenderTarget::mapCoordsToPixel(glm::vec2 point) const {
  return mapCoordsToPixel(point, getView());
}
}
