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
  Color(int red, int green, int blue, int alpha = 255) noexcept;
  Color(float red, float green, float blue, float alpha = 1.0f) noexcept;

  /// Creates a color from a text with an hexadecimal value "#RRGGBB"
  /// \param text Text containing an hexadecimal value
  /// \return the corresponding color
  static Color parseHex6(const char *text);

  /// Creates a color from an 32-bit unsigned integer
  /// \param color Value containing the RGBA color
  /// \return
  static Color fromRgb(std::uint32_t color);

public:
  float r{0.0f};
  float g{0.0f};
  float b{0.0f};
  float a{1.0f};
};

float *value_ptr(Color& color);

/// Compares 2 colors
/// \param left Left color operand
/// \param right Right operand color
/// \return true if they are equals, false otherwise
bool operator==(const Color &left, const Color &right);
bool operator!=(const Color &left, const Color &right);
Color operator+(const Color &left, const Color &right);
Color operator-(const Color &left, const Color &right);
Color operator*(const Color &left, const Color &right);
Color operator*(const Color &left, float value);
Color operator*(float left, const Color& right);
Color &operator+=(Color &left, const Color &right);
Color &operator-=(Color &left, const Color &right);
Color &operator*=(Color &left, const Color &right);
}
