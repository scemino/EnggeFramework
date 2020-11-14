#pragma once
#include <iterator>
#include <string>
#include <glm/vec2.hpp>
#include <ngf/Graphics/Rect.h>
#include <ngf/System/DisplayMode.h>

namespace ngf {
class VideoDisplay {
private:
  friend class VideoDisplayIterator;

private:
  explicit VideoDisplay(int id);

public:
  /// @brief Gets the name of the video display.
  /// \return The name of the video display.
  [[nodiscard]] std::string getName() const;
  /// @brief Gets the bounds of the video display.
  /// \return The bounds of the video display.
  [[nodiscard]] ngf::irect getBounds() const;
  /// @brief Gets the id of the video display.
  /// \return The id of the video display.
  [[nodiscard]] int getId() const;
  /// @brief Gets the display modes of the video display.
  /// \return The display modes of the video display.
  [[nodiscard]] DisplayModeIterator getDisplayModes() const;

private:
  int m_id{0};
};

class VideoDisplayIterator {
public:
  using value_type = VideoDisplay;
  using pointer = const VideoDisplay *;
  using reference = const VideoDisplay &;
  using iterator_category = std::forward_iterator_tag;

  VideoDisplayIterator();
  explicit VideoDisplayIterator(int id);

  [[nodiscard]] VideoDisplayIterator begin() const;
  [[nodiscard]] VideoDisplayIterator end() const;

  [[nodiscard]] VideoDisplayIterator cbegin() const;
  [[nodiscard]] VideoDisplayIterator cend() const;

  const VideoDisplayIterator operator++(int);
  VideoDisplayIterator &operator++();

  bool operator==(const VideoDisplayIterator &rhs) const;
  bool operator!=(const VideoDisplayIterator &rhs) const;

  reference operator*();
  pointer operator->();

private:
  VideoDisplay m_display;
};
}
