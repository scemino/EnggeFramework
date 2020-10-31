#pragma once
#include <vector>
#include <ngf/Graphics/Rect.h>
#include <ngf/Graphics/Sprite.h>
#include <ngf/System/TimeSpan.h>

namespace ngf {

/// @brief This is a frame of an animation.
/// @sa ngf::Animation
struct AnimationFrame {
  const Texture *texture; ///< Texture used for this animation frame.
  irect rect;             ///< Rectangle of texture to use for this animation frame.
};

/// @brief An animation is a collection of frames that are displayed
/// consecutively with a specfied fps (Frames Per Second).
class Animation {
public:
  using iterator = std::vector<AnimationFrame>::iterator;
  using const_iterator = std::vector<AnimationFrame>::const_iterator;
  using size_type = std::vector<AnimationFrame>::size_type;
  using reference = std::vector<AnimationFrame>::reference;
  using const_reference = std::vector<AnimationFrame>::const_reference;

  /// @brief Adds a frame to the animation.
  /// \param frame The frame to add.
  void addFrame(const AnimationFrame &frame);

  /// @brief Adds frames from several parts of a texture.
  /// \param texture Texture from where to extract the frames.
  /// \param frameSize Size of a frame.
  /// \param start Coordinates of the first frame in the texture.
  /// \param layout Size of the layout of the frames.
  /// \param frameCount Number of frames to extract.
  void addFrames(const Texture &texture, const glm::ivec2 &frameSize, const glm::ivec2 &start,
                 const glm::ivec2 &layout, size_t frameCount);

  /// @brief Sets the fps (Frames Per Second) of the animation.
  /// \param fps FPS (Frames Per Second) to use for the animation.
  void setFps(float fps) { m_fps = fps; }
  /// @brief Gets the fps (Frames Per Second) of the animation.
  /// \return The FPS (Frames Per Second) used for the animation.
  [[nodiscard]] float getFps() const { return m_fps; }

  /// @brief Gets an iterator to the first element of the frames.
  /// \return The iterator to the first element of the frames.
  iterator begin() noexcept { return m_frames.begin(); }
  /// @brief Gets an iterator to the element following the last element of the array.
  /// \return The iterator to the element following the last element of the array.
  iterator end() noexcept { return m_frames.end(); }

  /// @brief Gets the constant version of an iterator to the first element of the frames.
  /// \return The constant version of an iterator to the first element of the frames.
  [[nodiscard]] const_iterator cbegin() const noexcept { return m_frames.cbegin(); }
  /// @brief Gets the constant version of an iterator to the element following the last element of the array.
  /// \return The constant version of an iterator to the element following the last element of the array.
  [[nodiscard]] const_iterator cend() const noexcept { return m_frames.cend(); }

  /// @brief Gets the reference to a frame at the specified index.
  /// \param index Index of the frame to get.
  /// \return The reference to a frame at the specified index.
  [[nodiscard]] reference at(size_type index) { return m_frames.at(index); }
  /// @brief Gets the constant reference to a frame at the specified index.
  /// \param index Index of the frame to get.
  /// \return The constant reference to a frame at the specified index.
  [[nodiscard]] const_reference at(size_type index) const { return m_frames.at(index); }

  /// @brief Indicates whether or not the animation is empty (no frame).
  /// \return true if the animation contains no frame, false otherwise.
  [[nodiscard]] bool empty() const noexcept { return m_frames.empty(); }
  /// @brief Gets the number of frames in the current animation.
  /// \return The number of frames in the current animation.
  [[nodiscard]] size_type size() const noexcept { return m_frames.size(); }

private:
  float m_fps{10.f};
  std::vector<AnimationFrame> m_frames;
};
}
