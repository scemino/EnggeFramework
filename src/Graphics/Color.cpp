#include <ngf/Graphics/Color.h>
#include <algorithm>

namespace ngf {
Color::Color(
    int red, int green, int blue, int alpha) noexcept
    : r(static_cast<float>(red) / 255.f),
      g(static_cast<float>(green) / 255.f),
      b(static_cast<float>(blue) / 255.f),
      a(static_cast<float>(alpha) / 255.f) {
}

Color::Color(float red, float green, float blue, float alpha) noexcept
    : r(red), g(green), b(blue), a(alpha) {
}

bool operator==(const Color &left, const Color &right) {
  return (left.r == right.r) && (left.g == right.g) && (left.b == right.b) &&
      (left.a == right.a);
}

bool operator!=(const Color &left, const Color &right) {
  return !(left == right);
}

Color operator+(const Color &left, const Color &right) {
  return Color(std::min(left.r + right.r, 1.f),
               std::min(left.g + right.g, 1.f),
               std::min(left.b + right.b, 1.f),
               std::min(left.a + right.a, 1.f));
}

Color operator-(const Color &left, const Color &right) {
  return Color(std::max(left.r - right.r, 0.0f),
               std::max(left.g - right.g, 0.0f),
               std::max(left.b - right.b, 0.0f),
               std::max(left.a - right.a, 0.0f));
}

Color operator*(const Color &left, const Color &right) {
  return Color(left.r * right.r,
               left.g * right.g,
               left.b * right.b,
               left.a * right.a);
}

Color operator*(const Color &left, float right) {
  return Color(std::clamp(left.r * right, 0.f, 1.f),
               std::clamp(left.g * right, 0.f, 1.f),
               std::clamp(left.b * right, 0.f, 1.f),
               std::clamp(left.a * right, 0.f, 1.f));
}

Color operator*(float left, const Color& right) {
  return Color(std::clamp(right.r * left, 0.f, 1.f),
               std::clamp(right.g * left, 0.f, 1.f),
               std::clamp(right.b * left, 0.f, 1.f),
               std::clamp(right.a * left, 0.f, 1.f));
}

Color &operator+=(Color &left, const Color &right) {
  return left = left + right;
}

Color &operator-=(Color &left, const Color &right) {
  return left = left - right;
}

Color &operator*=(Color &left, const Color &right) {
  return left = left * right;
}

Color Color::parseHex6(const char *text) {
  auto c = std::strtol(text, nullptr, 16);
  return fromRgb(c);
}

Color Color::fromRgb(unsigned int color) {
  auto col = static_cast<unsigned int>(color);
  Color c(static_cast<int>((col >> 16u) & 255u),
          static_cast<int>((col >> 8u) & 255u),
          static_cast<int>(col & 255u));
  return c;
}

float *value_ptr(Color &color) {
  return &(color.r);
}

const float *value_ptr(const Color &color) {
  return &(color.r);
}
}
