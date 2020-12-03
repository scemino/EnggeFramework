#pragma once
#include <vector>

namespace ngf {
class Base85 {
public:
  static std::vector<char> decode(const char *src) {
    auto decodedSize = (((int) strlen(src) + 4) / 5) * 4;
    std::vector<char> decoded(decodedSize);
    decode(src, decoded.data());
    return decoded;
  }

  static std::vector<char> encode(const char *src, size_t len) {
    auto dstSize = (((int) len + 3) / 4 * 5 + 1);
    std::vector<char> dst(dstSize);
    encode(src, dstSize, dst.data());
    return dst;
  }

private:
  static void encode(const char *src, size_t len, char *dst) {
    for (int i = 0; i < len; i += 4) {
      unsigned int d = *(unsigned int *) (src + i);
      for (int n5 = 0; n5 < 5; n5++, d /= 85) {
        char c = encodeByte(d);
        *dst++ = c;
      }
    }
  }

  static void decode(const char *src, char *dst) {
    while (*src) {
      unsigned int tmp =
          decodeByte(src[0]) +
              85 * (decodeByte(src[1]) +
                  85 * (decodeByte(src[2]) +
                      85 * (decodeByte(src[3]) +
                          85 * decodeByte(src[4]))));
      dst[0] = ((tmp >> 0) & 0xFF);
      dst[1] = ((tmp >> 8) & 0xFF);
      dst[2] = ((tmp >> 16) & 0xFF);
      dst[3] = ((tmp >> 24) & 0xFF);
      src += 5;
      dst += 4;
    }
  }

  static unsigned int decodeByte(char c) {
    return c >= '\\' ? c - 36 : c - 35;
  }

  static char encodeByte(unsigned int x) {
    x = (x % 85) + 35;
    return (x >= '\\') ? static_cast<char>(x + 1) : static_cast<char>(x);
  }
};
}