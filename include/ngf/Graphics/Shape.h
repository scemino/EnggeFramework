#pragma once
#include <ngf/Graphics/Drawable.h>
#include <ngf/Math/Transform.h>
#include <ngf/Math/VectorExtensions.h>

namespace ngf {

/// @brief Base class for textured shapes.
/// @sa CircleShape
class Shape : public Drawable {
public:
  Shape()
      : m_textureRect(ngf::frect::fromPositionSize({0.0f, 0.0f}, {1.0f, 1.0f})) {
  }
  /// @brief Destructor of a Shape object.
  ~Shape() override = default;

  /// @brief Gets the transformation of the shape.
  /// \return The transformation of the shape.
  Transform &getTransform() { return m_transform; }

  /// @brief Gets the read-only transformation of the shape.
  /// \return The read-only transformation of the shape.
  const Transform &getTransform() const { return m_transform; }

  /// @brief Sets the texture to use for this shape.
  /// \param texture Texture used to fill the shape.
  /// \param resetRect Indicates whether the texture rectangle has to be be reset to the size of the new texture.
  void setTexture(const Texture &texture, bool resetRect) {
    m_texture = &texture;

    if (resetRect) {
      m_textureRect = ngf::frect::fromPositionSize({0.0f, 0.0f}, {1.0f, 1.0f});
      updateTexCoords();
    }
  }

  /// Sets the rectangle to use in the texture, in pixels.
  /// \param rect Rectangle to use in the texture, in pixels.
  void setTextureRect(const frect &rect) {
    m_textureRect = rect;
    updateTexCoords();
  }

  /// @brief Sets the color of the shape.
  /// \param color Color to use.
  void setColor(const Color &color) {
    m_color = color;
    updateColors();
  }

  /// @brief Gets the color of the sprite.
  /// \return The color of the sprite.
  Color getColor() const{
    return m_color;
  }

  /// @brief Gets the local bounding rectangle of the shape.
  /// \return The local bounding rectangle of the shape.
  frect getLocalBounds() const {
    return m_bounds;
  }

  /// @brief Sets the origin of the transformation of the shape.
  /// \param anchor The orgin of the shape.
  void setAnchor(Anchor anchor) {
    m_transform.setOriginFromAnchorAndBounds(anchor, getLocalBounds());
  }

  /// @brief Gets the total number of points of the shape.
  /// \return The total number of points of the shape.
  virtual std::size_t getPointCount() const = 0;
  /// &brief Gets a point of the shape.
  /// \param index Index of the point to get.
  /// \return Point at the specified index.
  virtual glm::vec2 getPoint(std::size_t index) const = 0;

  /// @brief Draws the shape to the target with the specified render states.
  /// \param target This is where the drawing is made (a window, a texture, etc.)
  /// \param states Render states to use to draw this shape.
  void draw(RenderTarget &target, RenderStates states) const override {
    RenderStates localStates = states;
    localStates.transform *= m_transform.getTransform();
    localStates.texture = m_texture;
    target.draw(PrimitiveType::TriangleFan, m_vertices, localStates);
  }

protected:
  void updateGeometry() {
    std::size_t count = getPointCount();
    assert(count >= 3);

    m_vertices.resize(count);

    auto min = glm::vec2(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    auto max = glm::vec2(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());

    for (std::size_t i = 0; i < count; ++i) {
      auto point = getPoint(i);
      m_vertices[i].pos = point;
      min = ngf::min(min, point);
      max = ngf::max(max, point);
    }

    updateColors();
    updateTexCoords();
  }

  void updateBounds(frect bounds) {
    m_bounds = bounds;
    updateTexCoords();
  }

private:
  void updateColors() {
    for (auto &l_vertex : m_vertices) {
      l_vertex.color = m_color;
    }
  }

  void updateTexCoords() {
    for (auto &l_vertex : m_vertices) {
      glm::vec2 ratio(0.0f, 0.0f);

      if (!m_bounds.isEmpty()) {
        ratio = (l_vertex.pos - m_bounds.getPosition()) / m_bounds.getSize();
      }

      l_vertex.texCoords = m_textureRect.getPosition() + m_textureRect.getSize() * ratio;
    }
  }

private:
  const Texture *m_texture{nullptr};
  std::vector<Vertex> m_vertices;
  frect m_bounds{};
  frect m_textureRect{};
  Color m_color{Colors::White};
  Transform m_transform;
};

}