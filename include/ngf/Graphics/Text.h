#pragma once
#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include <ngf/Graphics/Drawable.h>
#include <ngf/Graphics/FntFont.h>
#include <ngf/Graphics/Vertex.h>
#include "Math/Transform.h"

namespace ngf {
class RenderTarget;

// This code has been copied from SFML Text and adapted to use FntFont.

/// @brief Graphical text that can be drawn to a render target
class Text : public Drawable {
public:
  /// @brief Creates an empty text.
  Text();
  /// @brief Creates a text from a string, font and size.
  Text(std::wstring string, const Font &font, unsigned int characterSize = 30);

  /// @brief Sets the text's string.
  /// \param string The text's string.
  void setString(const std::string &string);
  /// @brief Gets the text's string.
  std::string getString() const;

  /// @brief Sets the text's string.
  /// \param string The text's string.
  void setWideString(const std::wstring &string);
  /// @brief Gets the text's string.
  std::wstring getWideString() const { return m_string; }

  /// @brief Sets the text's font.
  /// \param font The text's font.
  void setFont(const Font &font);
  /// @brief Gets the text's font.
  /// \return The current text's font.
  const Font *getFont() const;

  /// @brief Sets the maximum width allowed to display the text.
  /// \param maxWidth The maximum width allowed to display the text.
  void setMaxWidth(float maxWidth);
  /// @brief Gets the maximum width allowed to display the text.
  /// \return The maximum width allowed to display the text.
  float getMaxWidth() const { return m_maxWidth; }

  /// @brief Sets the color of the text.
  /// \param color The color of the text.
  void setColor(const Color &color);
  /// @brief Gets the color of the text.
  /// \return The color of the text.
  Color getColor() const { return m_color; }

  /// @brief Draws the text to the target with the specified render states.
  /// \param target This is where the drawing is made (a window, a texture, etc.)
  /// \param states Render states to use to draw this text.
  void draw(RenderTarget &target, RenderStates states = {}) const override;

  /// @brief Gets the local bounding rectangle of the text.
  /// \return The local bounding rectangle of the text.
  frect getLocalBounds() const;

  /// @brief Gets the transformation of the text.
  /// \return The transformation of the text.
  ngf::Transform &getTransform() { return m_transform; }
  /// @brief Gets the read-only transformation of the sprite.
  /// \return The read-only transformation of the sprite.
  const Transform &getTransform() const { return m_transform; }

private:
  void ensureGeometryUpdate() const;

private:
  ngf::Transform m_transform{};
  std::wstring m_string;
  const Font *m_font{nullptr};
  unsigned int m_characterSize{0};
  mutable Color m_color{Colors::White};
  mutable std::vector<Vertex> m_vertices;
  mutable frect m_bounds{};
  mutable bool m_geometryNeedUpdate{false};
  mutable const Texture *m_fontTexture{nullptr};
  float m_maxWidth{0};
};
}
