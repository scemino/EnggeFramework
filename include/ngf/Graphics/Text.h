#pragma once
#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include <ngf/Graphics/Drawable.h>
#include <ngf/Graphics/FntFont.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/Math/Transform.h>

namespace ngf {
class RenderTarget;

enum class Alignment {
  None,    ///< No alignement
  Left,    ///< Left alignement
  Right,   ///< Right alignement
  Center,  ///< Centered alignment
  Justify, ///< Justified alignment
};

// This code has been copied from Gamedev Framework (gf) and adapted to use FntFont.

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

  /// @brief Sets the alignement of the text.
  /// \param align The alignement of the text to set.
  void setAlignment(Alignment align);

  /// @brief Sets the character size.
  ///
  /// The default size is 30.
  /// \param characterSize Character size in pixels.
  void setCharacterSize(unsigned int characterSize);

  /// @brief Gets the character size.
  /// \return Character size in pixels.
  unsigned int getCharacterSize() const { return m_characterSize; }

  /// @brief Sets the line spacing factor.
  /// \param spacingFactor The size of the line spacing factor.
  void setLineSpacing(float spacingFactor);

  /// @brief Gets the size of the line spacing factor.
  /// \return The size of the line spacing factor.
  float getLineSpacing() const { return m_lineSpacingFactor; }

  /// @brief Sets the letter spacing factor.
  /// \param spacingFactor The size of the letter spacing factor.
  void setLetterSpacing(float spacingFactor);

  /// @brief Gets the size of the letter spacing factor.
  /// \return The size of the letter spacing factor.
  float getLetterSpacing() const { return m_letterSpacingFactor; }

  /// @brief Gets the alignment of the text
  /// \return The alignement of the text.
  Alignment getAlignment() const { return m_align; }

  /// @brief Sets the color of the text.
  /// \param color The color of the text.
  void setColor(const Color &color);

  /// @brief Gets the color of the text.
  /// \return The color of the text.
  Color getColor() const { return m_color; }

  /// @brief Sets the thickness of the text's outline
  /// \param thickness The thickness of the text's outline.
  void setOutlineThickness(float thickness);

  /// @brief Gets the thickness of the text's outline
  /// \return The thickness of the text's outline.
  float getOutlineThickness() const;

  /// @brief Sets the color of the outline.
  /// \param color The color of the outline.
  void setOutlineColor(const Color &color);

  /// @brief Gets the outline thickness of the text.
  /// \return The outline thickness of the text.
  Color getOutlineColor() const;

  /// @brief Draws the text to the target with the specified render states.
  /// \param target This is where the drawing is made (a window, a texture, etc.)
  /// \param states Render states to use to draw this text.
  void draw(RenderTarget &target, RenderStates states = {}) const override;

  /// @brief Gets the local bounding rectangle of the text.
  /// \return The local bounding rectangle of the text.
  frect getLocalBounds() const;

  /// @brief Sets the origin of the transformation of the text.
  /// \param anchor The orgin of the text.
  void setAnchor(Anchor anchor);

  /// @brief Gets the transformation of the text.
  /// \return The transformation of the text.
  ngf::Transform &getTransform() { return m_transform; }

  /// @brief Gets the read-only transformation of the sprite.
  /// \return The read-only transformation of the sprite.
  const Transform &getTransform() const { return m_transform; }

private:
  void ensureGeometryUpdate();

private:
  ngf::Transform m_transform{};
  std::wstring m_string;
  const Font *m_font{nullptr};
  unsigned int m_characterSize{30};
  float m_letterSpacingFactor{1.0f};
  Color m_color{Colors::White};
  Color m_outlineColor{Colors::White};
  std::vector<Vertex> m_vertices;
  float m_outlineThickness{0.f};
  std::vector<Vertex> m_outlineVertices;
  float m_lineSpacingFactor{1.0f};
  Alignment m_align{Alignment::Center};
  frect m_bounds{};
  const Texture *m_fontTexture{nullptr};
  float m_maxWidth{0};
};
}
