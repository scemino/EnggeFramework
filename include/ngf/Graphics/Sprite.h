#pragma once
#include <array>
#include <ngf/Graphics/Anchor.h>
#include <ngf/Graphics/Drawable.h>
#include <ngf/Graphics/Rect.h>
#include <ngf/Graphics/RenderStates.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Texture.h>
#include <ngf/Math/Transform.h>
#include <ngf/Graphics/Vertex.h>

namespace ngf {
/// @brief A sprite is a rectangle with a texture, a transformation and a color.
/// @sa ngf::Texture, ngf::Transform
class Sprite : public Drawable {
public:
  /// @brief Creates an empty sprite, with no texture.
  Sprite() = default;

  /// @brief Creates a sprite with a specified texture.
  /// \param texture Texture used to fill the sprite.
  explicit Sprite(const Texture &texture);

  /// @brief Creates a sprite with a texture, just a part of the texture will be used, specified by the 'textureRect' parameter in pixels.
  /// \param texture Texture used to fill the sprite.
  /// \param textureRect Rectangle to use in the texture, in pixels.
  Sprite(const Texture &texture, const irect &textureRect);

  /// @brief Creates a sprite with a texture, just a part of the texture will be used, specified by the 'textureRect' parameter.
  /// \param texture Texture used to fill the sprite.
  /// \param textureRect Rectangle to use in the texture, the value is relative to the texture size [0.5,1] indicates to use only the half of the texture in width and the whole texture in height.
  Sprite(const Texture &texture, const frect &textureRect);

  /// @brief Destructor of a Sprite.
  ~Sprite() override = default;

  /// @brief Sets the texture to use for this sprite.
  /// \param texture Texture used to fill the sprite.
  /// \param resetRect Indicates whether the texture rectangle has to be be reset to the size of the new texture.
  void setTexture(const Texture &texture, bool resetRect = false);

  /// Sets the rectangle to use in the texture, in pixels.
  /// \param rect Rectangle to use in the texture, in pixels.
  void setTextureRect(irect rect);

  /// @brief Sets the color of the sprite.
  /// \param color Color to use.
  void setColor(const Color &color);

  /// @brief Gets the color of the sprite.
  /// \return The color of the sprite.
  Color getColor() const;

  /// @brief Gets the local bounding rectangle of the sprite.
  /// \return The local bounding rectangle of the sprite.
  frect getLocalBounds() const { return m_bounds; }

  /// @brief Gets the transformation of the sprite.
  /// \return The transformation of the sprite.
  Transform &getTransform() { return m_transform; }

  /// @brief Gets the read-only transformation of the sprite.
  /// \return The read-only transformation of the sprite.
  const Transform &getTransform() const { return m_transform; }

  /// @brief Sets the origin of the transformation of the sprite.
  /// \param anchor The orgin of the sprite.
  void setAnchor(Anchor anchor);

  /// @brief Draws the sprite to the target with the specified render states.
  /// \param target This is where the drawing is made (a window, a texture, etc.)
  /// \param states Render states to use to draw this sprite.
  void draw(RenderTarget &target, RenderStates states = {}) const override;

  /// @brief Flips the sprite texture horizontally.
  void setFlipX(bool flip = true);
  bool getFlipX() const;
  /// @brief Flips the sprite texture vertically
  void setFlipY(bool flip = true);
  bool getFlipY() const;

private:
  void updateGeometry();

private:
  const Texture *m_texture{nullptr};
  frect m_textureRect{frect::fromMinMax({0.f, 0.f}, {1.f, 1.f})};
  frect m_bounds{};
  std::array<Vertex, 4> m_vertices{};
  Transform m_transform;
  bool m_flipX{false};
  bool m_flipY{false};
};
}
