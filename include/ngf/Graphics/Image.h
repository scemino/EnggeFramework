#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include <glm/vec2.hpp>

namespace ngf {
class Image {
public:
  Image();
  Image(glm::uvec2 size, const std::uint8_t* pixels);

  bool load(std::istream &is);
  bool loadFromMemory(const void* data, std::size_t dataSize);

  [[nodiscard]] glm::uvec2 getSize() const;
  [[nodiscard]] int getBpp() const { return m_bpp; }
  [[nodiscard]] const std::uint8_t *getPixelsPtr() const;
  [[nodiscard]] std::uint8_t *getPixelsPtr();

private:
  glm::uvec2 m_size{0, 0};
  std::vector<std::uint8_t> m_pixels{};
  int m_bpp{0};
};
}
