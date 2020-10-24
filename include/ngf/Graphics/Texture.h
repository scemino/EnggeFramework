#pragma once
#include <cassert>
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <filesystem>
#include "Image.h"

namespace ngf {
class Texture {
public:
  enum class Format {
    Rgba,
    Rgb,
    Alpha,
  };

  enum class Type {
    Texture1D,
    Texture2D,
  };

  explicit Texture(Type type = Type::Texture2D, Format format = Format::Rgba);
  explicit Texture(const Image &image);
  explicit Texture(const std::filesystem::path& path);
  Texture(Format format, glm::uvec2 size, const void *data);
  Texture(Format format, int width, const void *data);
  ~Texture();

  void load(const std::filesystem::path& path);
  void setData(glm::uvec2 size, const void *data);

  void setSmooth(bool smooth = true);
  [[nodiscard]] bool isSmooth() const noexcept;
  [[nodiscard]] glm::uvec2 getSize() const noexcept { return m_size; }

  static void bind(const Texture *pTexture);

private:
  void updateFilters();

private:
  Type m_type{Type::Texture2D};
  Format m_format{Format::Rgba};
  glm::uvec2 m_size{0, 0};
  bool m_smooth{false};
  unsigned int m_img_tex{0};
};
}
