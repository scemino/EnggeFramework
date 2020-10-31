#include <ngf/Graphics/AnimatedSprite.h>
#include <ngf/Graphics/Animation.h>

namespace ngf {
void AnimatedSprite::setAnimation(Animation *animation) {
  m_animation = animation;
  m_index = 0;
  if (!m_animation || m_animation->empty())
    return;
  m_sprite.setTexture(*m_animation->at(m_index).texture);
  m_sprite.setTextureRect(m_animation->at(m_index).rect);
}

void AnimatedSprite::update(const TimeSpan &elapsed) {
  if (!m_animation)
    return;
  auto fps = m_animation->getFps();
  if (!m_animation || m_animation->empty())
    return;
  m_elapsed += elapsed;
  const auto step = 1.f / fps;
  if (m_elapsed.getTotalSeconds() > step) {
    m_elapsed -= TimeSpan::seconds(step);
    m_index = (m_index + 1) % m_animation->size();
    m_sprite.setTexture(*m_animation->at(m_index).texture);
    m_sprite.setTextureRect(m_animation->at(m_index).rect);
  }
}

void AnimatedSprite::draw(RenderTarget &target, RenderStates states) {
  if (!m_animation)
    return;
  m_sprite.draw(target, states);
}
}
