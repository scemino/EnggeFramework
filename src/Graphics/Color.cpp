#include <ngf/Graphics/Color.h>
#include <algorithm>

namespace ngf {
Color::Color(
    std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha) noexcept
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
  return Color(std::uint8_t(std::min(int(left.r) + right.r, 255)),
               std::uint8_t(std::min(int(left.g) + right.g, 255)),
               std::uint8_t(std::min(int(left.b) + right.b, 255)),
               std::uint8_t(std::min(int(left.a) + right.a, 255)));
}

Color operator-(const Color &left, const Color &right) {
  return Color(std::uint8_t(std::max(int(left.r) - right.r, 0)),
               std::uint8_t(std::max(int(left.g) - right.g, 0)),
               std::uint8_t(std::max(int(left.b) - right.b, 0)),
               std::uint8_t(std::max(int(left.a) - right.a, 0)));
}

Color operator*(const Color &left, const Color &right) {
  return Color(std::uint8_t(int(left.r) * right.r / 255),
               std::uint8_t(int(left.g) * right.g / 255),
               std::uint8_t(int(left.b) * right.b / 255),
               std::uint8_t(int(left.a) * right.a / 255));
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
  Color c((col >> 16u) & 255u, (col >> 8u) & 255u, col & 255u);
  return c;
}
}
