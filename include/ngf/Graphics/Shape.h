#pragma once
#include <ngf/Graphics/Drawable.h>
#include <ngf/Math/Transform.h>
#include <ngf/Math/VectorExtensions.h>

namespace ngf {

/// @brief Base class for textured shapes.
/// @sa CircleShape, ConvexShape, RectangleShape
class Shape : public Drawable {
public:
  Shape();
  /// @brief Destructor of a Shape object.
  ~Shape() override;

  /// @brief Gets the transformation of the shape.
  /// \return The transformation of the shape.
  Transform &getTransform();

  /// @brief Gets the read-only transformation of the shape.
  /// \return The read-only transformation of the shape.
  const Transform &getTransform() const;

  /// @brief Sets the texture to use for this shape.
  /// \param texture Texture used to fill the shape.
  /// \param resetRect Indicates whether the texture rectangle has to be be reset to the size of the new texture.
  void setTexture(const Texture &texture, bool resetRect);

  /// Sets the rectangle to use in the texture, in pixels.
  /// \param rect Rectangle to use in the texture, in pixels.
  void setTextureRect(const frect &rect);

  /// @brief Sets the color of the shape.
  /// \param color Color to use.
  void setColor(const Color &color);

  /// @brief Gets the color of the sprite.
  /// \return The color of the sprite.
  Color getColor() const;

  /// @brief Sets the outline color of the shape.
  /// \param color The outline color of the shape.
  void setOutlineColor(const Color &color);

  /// @brief Gets the outline color of the shape.
  /// \return The outline color of the shape.
  Color getOutlineColor() const { return m_outlineColor; }

  /// @brief Sets the thickness of the shape's outline.
  /// \param thickness The thickness of the shape's outline.
  void setOutlineThickness(float thickness);

  /// @brief Gets the thickness of the shape's outline.
  /// \return the thickness of the shape's outline.
  float getOutlineThickness() const { return m_outlineThickness; }

  /// @brief Gets the local bounding rectangle of the shape.
  /// \return The local bounding rectangle of the shape.
  frect getLocalBounds() const;

  /// @brief Sets the origin of the transformation of the shape.
  /// \param anchor The orgin of the shape.
  void setAnchor(Anchor anchor);

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
  void draw(RenderTarget &target, RenderStates states) const override;

protected:
  void updateGeometry();
  void updateBounds(frect bounds);
  void updateAutoBounds();

private:
  void updateColors();
  void updateTexCoords();
  void updateOutline();
  void updateOutlineColors();

private:
  const Texture *m_texture{nullptr};
  std::vector<Vertex> m_vertices;
  std::vector<Vertex> m_outlineVertices;
  frect m_bounds{};
  frect m_textureRect{};
  Color m_color{Colors::White};
  Color m_outlineColor{Colors::White};
  float m_outlineThickness{0.f};
  Transform m_transform;
};

}