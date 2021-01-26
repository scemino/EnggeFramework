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
    R"(#version 100
attribute vec2 a_position;
attribute vec4 a_color;
attribute vec2 a_texCoords;
varying vec4 v_color;
varying vec2 v_texCoords;
uniform mat3 u_transform;

void main(void) {
  v_texCoords = a_texCoords;
  v_color = a_color;
  vec3 worldPosition = vec3(a_position, 1);
  vec3 normalizedPosition = worldPosition * u_transform;
  gl_Position = vec4(normalizedPosition.xy, 0, 1);
  gl_PointSize = 1.0;
})";

const char *fragmentShaderSource =
    R"(#version 100
precision mediump float;
varying vec4 v_color;
varying vec2 v_texCoords;
uniform sampler2D u_texture;

void main(void) {
  vec4 color = texture2D(u_texture, v_texCoords);
  gl_FragColor = color * v_color;
}
)";

const char *alphaFragmentShaderSource =
    R"(#version 100
precision mediump float;
varying vec4 v_color;
varying vec2 v_texCoords;
uniform sampler2D u_texture;

void main(void) {
  vec4 color = texture2D(u_texture, v_texCoords);
  gl_FragColor = vec4(v_color.xyz, v_color.a * color.a);
}
)";
}

RenderTarget::RenderTarget(glm::ivec2 size)
    : m_view(frect::fromPositionSize({0.0f, 0.0f}, {static_cast<float>(size.x), static_cast<float>(size.y)})),
      m_emptyTexture(createWhitePixel()) {
  m_defaultShader.load(vertexShaderSource, fragmentShaderSource);
  m_defaultAlphaShader.load(vertexShaderSource, alphaFragmentShaderSource);
}

void RenderTarget::ensureActive() {
  GLint boundFrameBuffer;
  GL_CHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFrameBuffer));
  if (boundFrameBuffer != (GLint) getHandle()) {
    activate();
  }
}

void RenderTarget::clear(const Color &color) {
  ensureActive();
  GL_CHECK(glClearColor(color.r, color.g, color.b, color.a));
  GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
}

void RenderTarget::drawCore(PrimitiveType primitiveType,
                            size_t sizeVertices,
                            const std::uint16_t *indices,
                            size_t sizeIndices,
                            RenderStates states) {
  ensureActive();

  // set texture
  auto pTexture = states.texture;
  if (!pTexture) {
    pTexture = &m_emptyTexture;
  }

  // set shader
  auto shader = states.shader;
  if (!shader) {
    shader = pTexture->getFormat() == Texture::Format::Alpha ? &m_defaultAlphaShader : &m_defaultShader;
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

  ngf::Shader::bind(shader);

  for (auto info : attributes) {
    auto loc = shader->getAttributeLocation(info.name);
    if (loc == -1)
      continue;

    GL_CHECK(glEnableVertexAttribArray(loc));
    const void *pointer = reinterpret_cast<const void *>(info.offset);
    GL_CHECK(glVertexAttribPointer(loc,
                                   info.size,
                                   static_cast<GLenum>(info.type),
                                   info.normalized ? GL_TRUE : GL_FALSE,
                                   sizeof(Vertex),
                                   pointer));
  }

  // draw
  if (indices) {
    GL_CHECK(glDrawElements(getEnum(primitiveType), sizeIndices, GL_UNSIGNED_SHORT, nullptr));
  } else {
    GL_CHECK(glDrawArrays(getEnum(primitiveType), 0, sizeVertices));
  }

  for (auto info : attributes) {
    auto loc = shader->getAttributeLocation(info.name);
    GL_CHECK(glDisableVertexAttribArray(loc));
  }

  GL_CHECK(glDisable(GL_BLEND));
  ngf::Shader::bind(nullptr);
}

void RenderTarget::draw(PrimitiveType primitiveType,
                        const Vertex *vertices,
                        size_t sizeVertices,
                        RenderStates states) {
  ngf::VertexBuffer buf;
  constexpr size_t VertexSize = sizeof(Vertex);
  buf.buffer(ngf::VertexBuffer::Type::Array, sizeVertices * VertexSize, vertices);
  VertexBuffer::bind(&buf);

  drawCore(primitiveType, sizeVertices, nullptr, 0, states);
}

void RenderTarget::draw(PrimitiveType primitiveType,
                        const Vertex *vertices,
                        size_t sizeVertices,
                        const std::uint16_t *indices,
                        size_t sizeIndices,
                        RenderStates states) {
  ngf::VertexBuffer bufArray;
  constexpr size_t VertexSize = sizeof(Vertex);
  bufArray.buffer(ngf::VertexBuffer::Type::Array, sizeVertices * VertexSize, vertices);

  ngf::VertexBuffer bufElements;
  bufElements.buffer(ngf::VertexBuffer::Type::Element, sizeIndices * sizeof(uint16_t), indices);

  VertexBuffer::bind(&bufArray);
  VertexBuffer::bind(&bufElements);

  drawCore(primitiveType, sizeVertices, indices, sizeIndices, states);
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
  normalized.x = 2.0f * static_cast<float>(point.x - viewport.min.x) / viewport.getWidth() - 1;
  normalized.y = 1 - 2.0f * static_cast<float>(point.y - viewport.min.y) / viewport.getHeight();

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

Image RenderTarget::captureFramebuffer(unsigned int name) const {
  auto size = getSize();
  std::vector<uint8_t> pixels(static_cast<std::size_t>(size.x) * static_cast<std::size_t>(size.y) * 4);

  GLint boundFrameBuffer;
  GL_CHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &boundFrameBuffer));

  if (static_cast<unsigned>(boundFrameBuffer) != name) {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, name));
  }

  GL_CHECK(glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()));

  if (static_cast<unsigned int>(boundFrameBuffer) != name) {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, boundFrameBuffer));
  }

  Image image(size, pixels.data());
  return image;
}
}
