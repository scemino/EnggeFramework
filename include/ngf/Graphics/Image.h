#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include <glm/vec2.hpp>

namespace ngf {
/// @brief An image is an array of pixels.
///
/// This class is used to load an image from memory or a file.
/// @sa Texture
class Image {
public:
  /// @brief Creates an empty image.
  Image();
  /// @brief Creates an image from an array of pixels.
  /// \param size Size of the image to create.
  /// \param pixels Array of pixels to copy to the image.
  Image(glm::uvec2 size, const std::uint8_t* pixels);

  /// @brief Loads an image from a specified stream.
  /// \param input Stream where the image is defined, this can be a file or memory, etc.
  /// \return true if the image has been successfully loaded.
  bool load(std::istream &input);

  /// @brief Loads an image from a specified memory.
  /// \param data Memory where the image is defined, this can be a file or memory, etc.
  /// \return true if the image has been successfully loaded.
  bool loadFromMemory(const void* data, std::size_t dataSize);

  /// @brief Gets the size of the image (width x height).
  /// \return The size of the image.
  [[nodiscard]] glm::uvec2 getSize() const;
  /// @brief Gets the bpp (bytes per pixel) of the image.
  /// \return The bpp (bytes per pixel) of the image.
  [[nodiscard]] int getBpp() const { return m_bpp; }
  /// @brief Gets a read-only pointer to the array of pixels.
  /// \return A read-only pointer to the array of pixels.
  [[nodiscard]] const std::uint8_t *getPixelsPtr() const;
  /// @brief Gets a pointer to the array of pixels.
  /// \return A pointer to the array of pixels.
  [[nodiscard]] std::uint8_t *getPixelsPtr();

private:
  glm::uvec2 m_size{0, 0};
  std::vector<std::uint8_t> m_pixels{};
  int m_bpp{0};
};
}
