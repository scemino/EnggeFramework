#pragma once
#include <ngf/System/TimeSpan.h>
#include <ngf/Graphics/Sprite.h>

namespace ngf {
class Animation;
class AnimatedSprite {
public:
  void setAnimation(Animation *animation);
  void update(const TimeSpan &elapsed);
  void draw(RenderTarget &target, const RenderStates &states = {});

  Sprite &getSprite() { return m_sprite; }
  const Sprite &getSprite() const { return m_sprite; }

private:
  Sprite m_sprite;
  TimeSpan m_elapsed{};
  size_t m_index{0};
  Animation *m_animation{nullptr};
};
}