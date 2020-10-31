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
/// @brief PrimitiveType represents a kind of primitive to render.
/// @sa ngf::RenderTarget::draw
enum class PrimitiveType {
  Points,          ///< List of individual points.
  LineStrip,       ///< List of connected lines, a point uses the previous point to form a line.
  LineLoop,        ///< List of connected lines, a point uses the previous point to form a line, the last point is connected to the first.
  Lines,           ///< List of individual lines.
  TriangleStrip,   ///< List of connected triangles, a point uses the two previous points to form a triangle.
  TriangleFan,     ///< List of connected triangles, a point uses the common center and the previous point to form a triangle.
  Triangles,       ///< List of individual triangles.
};

/// @brief A render target is like a canvas, this where you draw the 2D objects: sprites, texts, etc.
class RenderTarget {
public:
  /// @brief Creates a render target for a specified window.
  /// \param window Window to create the render target for.
  explicit RenderTarget(Window &window);

  /// @brief Clears the entire target with a specified color.
  /// \param color Fill color to use to clear the render target.
  void clear(const Color &color);
  /// @brief Draws primitives defined by vertices and indices.
  /// \param primitiveType The type of primitives to draw.
  /// \param vertices The vertices used to draw.
  /// \param sizeVertices The number of vertices used to draw.
  /// \param indices The indices used to draw the promitives.
  /// \param sizeIndices The number of indices used to draw.
  /// \param states The render states to use for drawing.
  void draw(PrimitiveType primitiveType,
            const Vertex *vertices,
            size_t sizeVertices,
            const std::uint16_t *indices,
            size_t sizeIndices,
            RenderStates states = {});
  /// @brief Draws primitives defined by vertices.
  /// \param primitiveType The type of primitives to draw.
  /// \param vertices The vertices used to draw.
  /// \param sizeVertices The number of vertices used to draw.
  /// \param states The render states to use for drawing.
  void draw(PrimitiveType primitiveType,
            const Vertex *vertices,
            size_t sizeVertices,
            RenderStates states = {});
  /// @brief Draws primitives defined by vertices.
  /// \param primitiveType The type of primitives to draw.
  /// \param vertices The vertices container used to draw.
  /// \param states The render states to use for drawing.
  template<typename TContainer>
  void draw(PrimitiveType primitiveType,
            const TContainer &vertices,
            RenderStates states = {});

  /// @brief Sets the current active view.
  /// \param view The view to use.
  void setView(const View &view);
  /// @brief Gets the view currently uses by the render target.
  /// \return The view currently uses by the render target.
  [[nodiscard]] const View &getView() const { return m_view; }

  /// @brief Gets the canonical viewport of a view, applied to this render target.
  /// \param view The view for which we want to compute the viewport.
  /// \return The canonical viewport rectangle, expressed in pixels.
  [[nodiscard]] irect getCanonicalViewport(const View &view) const;

  /// @brief Gets the size of the rendering region of the target in pixels.
  /// \return The size of the rendering region of the target in pixels.
  [[nodiscard]] glm::ivec2 getSize() const;

  /// @brief Gets the viewport of a view, applied to this render target.
  /// \param view The view for which we want to compute the viewport.
  /// \return The viewport rectangle, expressed in pixels.
  [[nodiscard]] irect getViewport(const View &view) const;

  /// @brief Converts a point from target coordinates to world coordinates.
  /// \param point The point to convert.
  /// \param view The view to use for converting the point.
  /// \return The converted point, in world coordinates.
  [[nodiscard]] glm::vec2 mapPixelToCoords(glm::ivec2 point, const View &view) const;
  /// @brief Converts a point from target coordinates to world coordinates using the current view.
  /// \param point The point to convert.
  /// \return The converted point, in world coordinates.
  [[nodiscard]] glm::vec2 mapPixelToCoords(glm::ivec2 point) const;

  /// @brief Converts a point from world coordinates to target coordinates, using the current view
  /// \param point The point to convert.
  /// \param view The view to use for converting the point.
  /// \return The converted point, in target coordinates (pixels).
  [[nodiscard]] glm::ivec2 mapCoordsToPixel(glm::vec2 point, const View &view) const;
  /// @brief Converts a point from world coordinates to target coordinates, using the current view
  /// \param point The point to convert.
  /// \return The converted point, in target coordinates (pixels).
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
                        RenderStates states) {
  draw(primitiveType, vertices.data(), vertices.size(), states);
}
}
