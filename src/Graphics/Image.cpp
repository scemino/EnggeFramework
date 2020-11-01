#define STB_IMAGE_IMPLEMENTATION
#include <algorithm>
#include <stb_image.h>
#include "Image.h"

namespace ngf {

Image::Image() : m_size(0, 0) {
}

Image::Image(glm::uvec2 size, const std::uint8_t *pixels) : m_size(size) {
  m_size = size;
  m_bpp = 4;
  m_pixels.resize(size.x * size.y * m_bpp);
  memcpy(m_pixels.data(), pixels, m_pixels.size());
}

bool Image::load(std::istream &is) {
  is.seekg(0, std::ios::end);
  auto size = is.tellg();
  is.seekg(0, std::ios::beg);
  std::vector<char> data(size);
  is.read(data.data(), size);
  return loadFromMemory(data.data(), size);
}

bool Image::loadFromMemory(const void *data, std::size_t dataSize) {
  m_pixels.clear();

  // Load the image and get a pointer to the pixels in memory
  int width = 0;
  int height = 0;
  int channels = 0;
  const auto *buffer = static_cast<const unsigned char *>(data);
  auto *ptr = stbi_load_from_memory(buffer, static_cast<int>(dataSize), &width, &height, &channels, STBI_rgb_alpha);

  if (ptr) {
    // Assign the image properties
    m_size.x = width;
    m_size.y = height;
    m_bpp = 4;

    if (width && height) {
      // Copy the loaded pixels to the pixel buffer
      m_pixels.resize(width * height * 4);
      std::copy_n(ptr, m_pixels.size(), m_pixels.data());
    }

    // Free the loaded pixels (they are now in our own pixel buffer)
    stbi_image_free(ptr);

    return true;
  }

  // Error, failed to load the image
  std::cerr << "Failed to load image from memory. Reason: "
            << stbi_failure_reason() << std::endl;

  return false;
}

glm::uvec2 Image::getSize() const {
  return m_size;
}

const std::uint8_t *Image::getPixelsPtr() const {
  if (!m_pixels.empty()) {
    return &m_pixels[0];
  }
  std::cerr << "Trying to access the pixels of an empty image" << std::endl;
  return nullptr;
}

std::uint8_t *Image::getPixelsPtr() {
  if (!m_pixels.empty()) {
    return &m_pixels[0];
  }
  std::cerr << "Trying to access the pixels of an empty image" << std::endl;
  return nullptr;
}

}