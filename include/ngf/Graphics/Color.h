#pragma once
#include <cstdint>
#include <string>

namespace ngf {
/// @brief Utility class for manipulating RGBA colors (Red Green Blue Alpha)
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

  /// Creates a color with the specified RGBA components.
  /// \param red Red component between [0.f,1.f]
  /// \param green Green component between [0.f,1.f]
  /// \param blue Blue component between [0.f,1.f]
  /// \param alpha Alpha component between [0.f,1.f]
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

/// @brief Compares 2 colors for equality.
/// \param left Left color operand.
/// \param right Right operand color.
/// \return true if they are equals, false otherwise.
bool operator==(const Color &left, const Color &right);
/// @brief Compares 2 colors for inequality.
/// \param left Left color operand.
/// \param right Right operand color.
/// \return true if they are not equals, false otherwise.
bool operator!=(const Color &left, const Color &right);
/// @brief Adds 2 colors.
///
/// This operator returns the component-wise sum of two colors. Components that exceed 255 are clamped to 255.
/// \param left Left color operand.
/// \param right Right operand color.
/// \return The new color.
Color operator+(const Color &left, const Color &right);
/// @brief Subtracts 2 colors.
///
/// This operator returns the component-wise subtraction of two colors. Components below 0 are clamped to 0.
/// \param left Left color operand.
/// \param right Right operand color.
/// \return The new color.
Color operator-(const Color &left, const Color &right);
/// @brief Multiplies 2 colors.
///
/// This operator returns the component-wise multiplication (also called "modulation") of two colors.
/// Components are then divided by 255 so that the result is still in the range [0, 255].
/// \param left Left color operand.
/// \param right Right operand color.
/// \return The new color.
Color operator*(const Color &left, const Color &right);
Color operator*(const Color &left, float value);
Color operator*(float left, const Color& right);
/// @brief Multiplies 2 colors and assign the result to the left color.
///
/// This operator returns the component-wise multiplication (also called "modulation") of two colors, and assigns the result to the left operand. Components are then divided by 255 so that the result is still in the range [0, 255].
/// \param left
/// \param right
/// \return
Color &operator*=(Color &left, const Color &right);
/// @brief Adds 2 colors and assigns the result to left color.
///
/// This operator computes the component-wise sum of two colors, and assigns the result to the left operand.
/// Components that exceed 255 are clamped to 255.
/// \param left Left color operand.
/// \param right Right operand color.
/// \return The new color.
Color &operator+=(Color &left, const Color &right);
/// @brief Subtracts 2 colors and assigns the result to left color.
///
/// This operator computes the component-wise subtraction of two colors, and assigns the result to the left operand.
/// Components below 0 are clamped to 0.
/// \param left Left color operand.
/// \param right Right operand color.
/// \return The new color.
Color &operator-=(Color &left, const Color &right);
}
