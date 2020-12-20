#include "Object.h"
#include "Room.h"

void Object::play() {
  m_state = AnimState::Play;
  frameIndex = 0;
}
void Object::pause() { m_state = AnimState::Pause; }

void Object::update(const ngf::TimeSpan &e) {
  if (!visible)
    return;
  if (animations.empty())
    return;
  if (animationIndex < 0 || animationIndex >= animations.size())
    return;
  const auto &animation = animations[animationIndex];
  if (animation.frames.empty())
    return;

  if (frameIndex == -1) {
    frameIndex = animation.frames.size() - 1;
  }
  if (m_state == AnimState::Pause)
    return;

  if (frameIndex > animation.frames.size()) {
    frameIndex = 0;
    return;
  }
  elapsed += e;
  auto fps = animation.fps;
  if (fps == 0)
    fps = 10;
  const auto frameTime = 1.f / static_cast<float>(fps);
  if (elapsed.getTotalSeconds() > frameTime) {
    elapsed = ngf::TimeSpan::seconds(elapsed.getTotalSeconds() - frameTime);
    if (animation.loop || frameIndex != animation.frames.size() - 1) {
      frameIndex = (frameIndex + 1) % animation.frames.size();
    } else {
      pause();
    }
  }
}

void Object::draw(ngf::RenderTarget &target, ngf::RenderStates states) const {
  if (!visible)
    return;
  if (animations.empty())
    return;
  if (animationIndex >= animations.size())
    return;
  if (frameIndex < 0 || animations[animationIndex].frames.empty())
    return;

  ngf::RenderStates localStates = states;
  ngf::Transform t;
  t.setPosition({pos.x, room->getSize().y - pos.y});
  localStates.transform *= t.getTransform();

  const auto frame = animations[animationIndex].frames[frameIndex];
  auto texSize = texture->getSize();

  auto pShader = (LightingShader *) states.shader;
  pShader->setTexture(*texture);
  pShader->setContentSize(frame.sourceSize);
  pShader->setSpriteOffset({-frame.frame.getWidth() / 2.f + pos.x, -frame.frame.getHeight() / 2.f - pos.y});
  pShader->setSpritePosInSheet({static_cast<float>(frame.frame.min.x) / texSize.x,
                                static_cast<float>(frame.frame.min.y) / texSize.y});
  pShader->setSpriteSizeRelToSheet({static_cast<float>(frame.sourceSize.x) / texSize.x,
                                    static_cast<float>(frame.sourceSize.y) / texSize.y});

  glm::vec2 off = {
      frame.spriteSourceSize.min.x - frame.sourceSize.x / 2.f,
      frame.spriteSourceSize.min.y - frame.sourceSize.y / 2.f};
  ngf::Sprite sprite(*texture, frame.frame);
  sprite.getTransform().setPosition(off);
  sprite.draw(target, localStates);
}