#pragma once
#include <cstdint>
#include <string>

namespace ngf {
/*
 * Utility class for manipulating RGBA colors (Red Green Blue Alpha)
 */
class Color {
public:
  /// Creates a black color.
  Color() noexcept = default;

  /// Creates a color with the specified RGBA components.
  /// \param red Red component between [0,255]
  /// \param green Green component between [0,255]
  /// \param blue Blue component between [0,255]
  /// \param alpha Alpha component between [0,255]
  Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue,
        std::uint8_t alpha = 255) noexcept;

  /// Creates a color from a text with an hexadecimal value "#RRGGBB"
  /// \param text Text containing an hexadecimal value
  /// \return the corresponding color
  static Color parseHex6(const char *text);

  /// Creates a color from an 32-bit unsigned integer
  /// \param color Value containing the RGBA color
  /// \return
  static Color fromRgb(std::uint32_t color);

public:
  std::uint8_t r{0};
  std::uint8_t g{0};
  std::uint8_t b{0};
  std::uint8_t a{255};
};

/// Compares 2 colors
/// \param left Left color operand
/// \param right Right operand color
/// \return true if they are equals, false otherwise
bool operator==(const Color &left, const Color &right);
bool operator!=(const Color &left, const Color &right);
Color operator+(const Color &left, const Color &right);
Color operator-(const Color &left, const Color &right);
Color operator*(const Color &left, const Color &right);
Color &operator+=(Color &left, const Color &right);
Color &operator-=(Color &left, const Color &right);
Color &operator*=(Color &left, const Color &right);
}
