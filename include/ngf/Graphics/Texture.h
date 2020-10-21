#pragma once
#include <cassert>
#include <GL/glew.h>

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
  Texture(Format format, int width, int height, const void *data);
  Texture(Format format, int width, const void *data);
  ~Texture();

  void setSmooth(bool smooth = true);
  [[nodiscard]] bool isSmooth() const noexcept;

  void setData(int width, int height, const void *data) const;
  void bind() const;

private:
  void updateFilters();

private:
  Type m_type;
  Format m_format;
  bool m_smooth{false};
  unsigned int m_img_tex{0};
};
}
