#pragma once
#include <vector>
#include <ngf/Graphics/Rect.h>
#include <ngf/Graphics/Sprite.h>
#include <ngf/System/TimeSpan.h>

namespace ngf {

struct AnimationFrame {
  const Texture *texture;
  irect rect;
};

class Animation {
public:
  void setFps(float fps) { m_fps = fps; }
  [[nodiscard]] float getFps() const { return m_fps; }

  void addFrame(const AnimationFrame &frame);
  void addFrames(const Texture &texture, const glm::ivec2 &frameSize, const glm::ivec2 &start,
                 const glm::ivec2 &layout, size_t frameCount);

  [[nodiscard]] const std::vector<AnimationFrame> &getFrames() const { return m_frames; }

private:
  float m_fps{10.f};
  std::vector<AnimationFrame> m_frames;
};
}
