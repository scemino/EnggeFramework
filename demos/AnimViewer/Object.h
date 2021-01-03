#pragma once
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <ngf/Graphics/Texture.h>
#include <ngf/Graphics/Sprite.h>
#include "SpriteSheetItem.h"

struct ObjectAnimation {
  std::string name;
  std::vector<SpriteSheetItem> frames;
  std::vector<ObjectAnimation> layers;
  std::vector<glm::ivec2> offsets;
  std::vector<std::string> triggers;
  bool loop{false};
  int fps{0};
  int flags{0};
  int frameIndex{0};
  ngf::TimeSpan elapsed;
  bool visible{true};
};

enum class AnimState {
  Pause,
  Play,
  Stopped
};

class AnimControl {
public:
  void setAnimation(ObjectAnimation *anim) {
    m_anim = anim;
    stop();
  }

  void play(bool loop = false) {
    m_loop = loop;
    if (!m_anim)
      return;
    m_state = AnimState::Play;
    rewind(*m_anim);
  }

  void stop() {
    if (!m_anim)
      return;
    m_state = AnimState::Stopped;
    resetAnim(*m_anim);
  }

  void pause() { m_state = AnimState::Pause; }

  [[nodiscard]] AnimState getState() const { return m_state; }

  void update(const ngf::TimeSpan &e) {
    if (m_state != AnimState::Play)
      return;
    if (!m_anim)
      return;

    if (m_anim->frames.empty() && m_anim->layers.empty())
      return;

    if (!m_anim->frames.empty()) {
      update(e, *m_anim, 1);
      return;
    }

    for (auto &layer : m_anim->layers) {
      update(e, layer, 2);
    }
  }

private:
  static void resetAnim(ObjectAnimation &anim) {
    if (!anim.frames.empty()) {
      anim.frameIndex = static_cast<int>(anim.frames.size()) - 1;
    }
    if (!anim.layers.empty()) {
      std::for_each(anim.layers.begin(), anim.layers.end(), resetAnim);
    }
  }

  static void rewind(ObjectAnimation &anim) {
    if (!anim.frames.empty()) {
      anim.frameIndex = 0;
    }
    if (!anim.layers.empty()) {
      std::for_each(anim.layers.begin(), anim.layers.end(), rewind);
    }
  }

  void update(const ngf::TimeSpan &e, ObjectAnimation &animation, int depth) {
    animation.elapsed += e;
    auto fps = getFps(animation);
    assert(fps > 0);

    // frame time elapsed?
    const auto frameTime = 1.f / static_cast<float>(fps);
    if (animation.elapsed.getTotalSeconds() <= frameTime)
      return;

    animation.elapsed = ngf::TimeSpan::seconds(animation.elapsed.getTotalSeconds() - frameTime);
    animation.frameIndex++;

    // quit if animation length not reached
    if (animation.frameIndex != animation.frames.size())
      return;

    // loop if requested
    if (m_loop || animation.loop) {
      animation.frameIndex = 0;
      return;
    }

    // or stay at the last frame
    animation.frameIndex = animation.frameIndex - 1;
    if (depth == 1)
      m_state = AnimState::Stopped;
  }

  [[nodiscard]] static int getFps(const ObjectAnimation &animation) {
    if (animation.fps <= 0)
      return 10;
    return animation.fps;
  }

private:
  ObjectAnimation *m_anim{nullptr};
  AnimState m_state{AnimState::Pause};
  bool m_loop{false};
};

class AnimDrawable : public ngf::Drawable {
public:
  ~AnimDrawable() override = default;

  void setAnim(ObjectAnimation *anim) { m_anim = anim; }
  void setTexture(std::shared_ptr<ngf::Texture> texture) { m_texture = std::move(texture); }
  void setFlipX(bool flipX) { m_flipX = flipX; }

  void draw(ngf::RenderTarget &target, ngf::RenderStates states) const override {
    if (!m_anim)
      return;

    if (m_anim->frames.empty() && m_anim->layers.empty())
      return;

    draw(*m_anim, target, states);

    for (const auto &layer : m_anim->layers) {
      draw(layer, target, states);
    }
  }

private:
  void draw(const ObjectAnimation &anim, ngf::RenderTarget &target, ngf::RenderStates states) const {
    if (!anim.visible)
      return;
    if (anim.frames.empty())
      return;

    glm::ivec2 offset{0, 0};
    if (!anim.offsets.empty()) {
      offset = anim.offsets[anim.frameIndex];
    }
    const auto frame = anim.frames[anim.frameIndex];
    glm::vec2 origin = {frame.sourceSize.x / 2.f, frame.sourceSize.y / 2.f};
    glm::vec2 pos = {
        m_flipX ?
        frame.sourceSize.x - frame.spriteSourceSize.min.x - offset.x :
        frame.spriteSourceSize.min.x + offset.x,
        frame.spriteSourceSize.min.y - offset.y};

    ngf::Transform tFlipX;
    tFlipX.setScale({m_flipX ? -1 : 1, 1});
    ngf::Transform t;
    t.setOrigin(origin);
    t.setPosition(pos);
    states.transform = tFlipX.getTransform() * t.getTransform() * states.transform;

    ngf::Sprite sprite(*m_texture, frame.frame);
    sprite.draw(target, states);
  }

private:
  ObjectAnimation *m_anim{nullptr};
  std::shared_ptr<ngf::Texture> m_texture;
  bool m_flipX{false};
};

