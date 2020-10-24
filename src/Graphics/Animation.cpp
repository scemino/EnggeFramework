#include <ngf/Graphics/Animation.h>

namespace ngf {
void Animation::addFrame(const AnimationFrame &frame) { m_frames.push_back(frame); }

void Animation::addFrames(const Texture &texture, const glm::ivec2 &frameSize, const glm::ivec2 &start,
                          const glm::ivec2 &layout, size_t frameCount) {
  auto pos = start;
  size_t count = 0;
  for (auto x = 0; x < layout.x; ++x) {
    for (auto y = 0; y < layout.y; ++y) {
      if (count > frameCount)
        return;
      auto rect = irect::fromPositionSize(pos + glm::ivec2(x * frameSize.x, y * frameSize.y), frameSize);
      m_frames.push_back({&texture, rect});
      count++;
    }
  }
}
}
