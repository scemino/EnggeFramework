#include <ngf/Graphics/AnimatedSprite.h>
#include <ngf/Graphics/Animation.h>

namespace ngf {
void AnimatedSprite::setAnimation(Animation *animation) {
  m_animation = animation;
  m_index = 0;
  const auto &frames = m_animation->getFrames();
  if (frames.empty())
    return;
  m_sprite.setTexture(*frames[m_index].texture);
  m_sprite.setTextureRect(frames[m_index].rect);
}

void AnimatedSprite::update(const TimeSpan &elapsed) {
  if (!m_animation)
    return;
  auto fps = m_animation->getFps();
  const auto &frames = m_animation->getFrames();
  if (frames.empty())
    return;
  m_elapsed += elapsed;
  const auto step = 1.f / fps;
  if (m_elapsed.getTotalSeconds() > step) {
    m_elapsed -= TimeSpan::seconds(step);
    m_index = (m_index + 1) % frames.size();
    m_sprite.setTexture(*frames[m_index].texture);
    m_sprite.setTextureRect(frames[m_index].rect);
  }
}

void AnimatedSprite::draw(RenderTarget &target, const RenderStates &states) {
  if (!m_animation)
    return;
  m_sprite.draw(target, states);
}
}
