#pragma once
#include <ngf/Graphics/VertexBuffer.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/Graphics/VertexArray.h>
#include <ngf/Graphics/RenderStates.h>
#include <ngf/Graphics/View.h>
#include <ngf/Window/Window.h>
#include <glm/vec2.hpp>

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
  explicit RenderTarget(Window &window);

  void clear(const Color &color);
  void draw(PrimitiveType primitiveType,
            const Vertex *vertices,
            size_t sizeVertices,
            const std::uint16_t *indices,
            size_t sizeIndices,
            const RenderStates & = {});
  void draw(PrimitiveType primitiveType,
            const Vertex *vertices,
            size_t sizeVertices,
            const RenderStates & = {});
  template<typename TContainer>
  void draw(PrimitiveType primitiveType,
            const TContainer &vertices,
            const RenderStates & = {});

  void setView(const View &view);
  [[nodiscard]] const View &getView() const { return m_view; }

  [[nodiscard]] irect getCanonicalViewport(const View &view) const;
  [[nodiscard]] glm::ivec2 getSize() const;

  [[nodiscard]] irect getViewport(const View &view) const;

  [[nodiscard]] glm::vec2 mapPixelToCoords(glm::ivec2 point, const View &view) const;
  [[nodiscard]] glm::vec2 mapPixelToCoords(glm::ivec2 point) const;

  [[nodiscard]] glm::ivec2 mapCoordsToPixel(glm::vec2 point, const View &view) const;
  [[nodiscard]] glm::ivec2 mapCoordsToPixel(glm::vec2 point) const;

private:
  Window &m_window;
  glm::uvec2 m_size;
  View m_view;
  ngf::VertexArray m_vao;
  ngf::Shader m_defaultShader{};
  ngf::Texture m_emptyTexture{};
};

template<typename TContainer>
void RenderTarget::draw(PrimitiveType primitiveType,
                        const TContainer &vertices,
                        const RenderStates &states) {
  draw(primitiveType, vertices.data(), vertices.size(), states);
}
}
