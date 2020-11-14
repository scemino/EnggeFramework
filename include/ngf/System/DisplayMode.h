#pragma once
#include <glm/vec2.hpp>
#include <iterator>

namespace ngf {
/// @brief Enumeration of all the pixel formats of a display mode.
/// @sa DisplayMode
enum class DisplayModeFormat {
  Unknown,
  INDEX1LSB,
  INDEX1MSB,
  INDEX4LSB,
  INDEX4MSB,
  INDEX8,
  RGB332,
  RGB444,
  RGB555,
  BGR555,
  ARGB4444,
  RGBA4444,
  ABGR4444,
  BGRA4444,
  ARGB1555,
  RGBA5551,
  ABGR1555,
  BGRA5551,
  RGB565,
  BGR565,
  RGB24,
  BGR24,
  RGB888,
  RGBX8888,
  BGR888,
  BGRX8888,
  ARGB8888,
  RGBA8888,
  ABGR8888,
  BGRA8888,
  ARGB2101010,
  YV12,
  IYUV,
  YUY2,
  UYVY,
  YVYU,
  NV12,
  NV21,
};

/// @brief Gives information about a display mode: size, pixel format, bits per pixel, refresh rate.
/// @sa VideoDisplay
class DisplayMode {
private:
  friend class DisplayModeIterator;

private:
  DisplayMode(int displayId, int modeId);

private:
  int m_displayId{0};
  int m_modeId{0};

public:
  glm::ivec2 size; ///< Size in pixels
  int bitsPerPixel; ///< Bits per pixel
  int refreshRate; ///< Refresh rate in Hz
  DisplayModeFormat format; ///< Pixel format
};

class DisplayModeIterator {
public:
  using value_type = DisplayMode;
  using pointer = const DisplayMode *;
  using reference = const DisplayMode &;
  using iterator_category = std::forward_iterator_tag;

  explicit DisplayModeIterator(int displayId);
  explicit DisplayModeIterator(int displayId, int modeId);

  [[nodiscard]] DisplayModeIterator begin() const;
  [[nodiscard]] DisplayModeIterator end() const;

  [[nodiscard]] DisplayModeIterator cbegin() const;
  [[nodiscard]] DisplayModeIterator cend() const;

  const DisplayModeIterator operator++(int);
  DisplayModeIterator &operator++();

  bool operator==(const DisplayModeIterator &rhs) const;
  bool operator!=(const DisplayModeIterator &rhs) const;

  reference operator*();
  pointer operator->();

private:
  DisplayMode m_mode;
};
}