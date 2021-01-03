#include <algorithm>
#include <ngf/Graphics/Image.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

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

void Image::saveToFile (const std::filesystem::path& filename) const {
  std::string filenameString = filename.string();

  if (m_size.x == 0 || m_size.y == 0 || m_pixels.empty()) {
    std::cerr << "Could not save image to file: '" << filenameString.c_str() << "'\n";
    return;
  }

  stbi__flip_vertically_on_write = 1;
  auto ext = filename.extension();
  if (ext == ".png") {
    stbi_write_png(filenameString.c_str(), m_size.x, m_size.y, 4, m_pixels.data(), 0);
    return;
  }

  if (ext == ".jpg") {
    stbi_write_jpg(filenameString.c_str(), m_size.x, m_size.y, 4, m_pixels.data(), 0);
    return;
  }

  if (ext == ".bmp") {
    stbi_write_bmp(filenameString.c_str(), m_size.x, m_size.y, 4, m_pixels.data());
    return;
  }

  if (ext == ".tga") {
    stbi_write_tga(filenameString.c_str(), m_size.x, m_size.y, 4, m_pixels.data());
    return;
  }

  std::cerr << "Format not supported: '" << ext << "'\n";
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