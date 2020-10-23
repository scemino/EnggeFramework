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
  explicit Texture(const Image& image);
  Texture(Format format, glm::uvec2 size, const void *data);
  Texture(Format format, int width, const void *data);
  ~Texture();

  void setSmooth(bool smooth = true);
  [[nodiscard]] bool isSmooth() const noexcept;

  void load(std::filesystem::path path);
  void setData(glm::uvec2 size, const void *data) const;
  static void bind(const Texture* pTexture);

private:
  void updateFilters();

private:
  Type m_type;
  Format m_format;
  bool m_smooth{false};
  unsigned int m_img_tex{0};
};
}
