#pragma once
#include <memory>
#include <string>
#include <vector>
#include <ngf/Graphics/Drawable.h>
#include <ngf/Graphics/Rect.h>
#include <ngf/Graphics/RenderStates.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Sprite.h>
#include <ngf/Graphics/Texture.h>
#include <ngf/Math/Transform.h>
#include "SpriteSheetItem.h"

enum class Direction {
  None,
  Front,
  Back,
  Left,
  Right
};

enum class ObjectType {
  None,
  Prop,
  Spot,
  Trigger
};

enum class AnimState {
  Pause,
  Play
};

struct ObjectAnimation {
  std::string name;
  std::vector<SpriteSheetItem> frames;
  bool loop{false};
  double fps{0.f};
};

class Object : public ngf::Drawable {
public:
  std::shared_ptr<ngf::Texture> texture;
  std::string name{};
  bool visible{true};
  glm::ivec2 pos{0, 0};
  glm::ivec2 usePos{0, 0};
  Direction useDir{Direction::None};
  ngf::irect hotspot{};
  ObjectType type{ObjectType::None};
  std::vector<ObjectAnimation> animations;
  int animationIndex{0};
  int frameIndex{-1};
  int zsort{0};
  ngf::TimeSpan elapsed;

public:
  ~Object() override = default;

  void play() {
    m_state = AnimState::Play;
    frameIndex = 0;
  }
  void pause() { m_state = AnimState::Pause; }

  void update(const ngf::TimeSpan &e) {
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

  void draw(ngf::RenderTarget &target, ngf::RenderStates states) const override {
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
    t.setPosition({pos.x, pos.y});
    localStates.transform *= t.getTransform();

    const auto frame = animations[animationIndex].frames[frameIndex];
    glm::vec2 off = {
        frame.spriteSourceSize.min.x - frame.sourceSize.x / 2.f,
        frame.spriteSourceSize.min.y - frame.sourceSize.y / 2.f};
    ngf::Sprite sprite(*texture, frame.frame);
    sprite.getTransform().setPosition(off);
    sprite.draw(target, localStates);
  }

private:
  AnimState m_state{AnimState::Pause};
};