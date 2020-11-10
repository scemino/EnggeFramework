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
  Direction useDir{Direction::Front};
  ngf::irect hotspot{};
  ObjectType type{ObjectType::None};
  std::vector<ObjectAnimation> animations;
  int zsort{0};

public:
  ~Object() override = default;

  void draw(ngf::RenderTarget &target, ngf::RenderStates states) const override {
    if (!visible)
      return;
    if (animations.empty())
      return;
    if (animations[0].frames.empty())
      return;

    ngf::RenderStates localStates = states;
    ngf::Transform t;
    t.setPosition({pos.x, pos.y});
    localStates.transform *= t.getTransform();

    const auto frame = animations[0].frames.back().frame;
    ngf::Sprite sprite(*texture, frame);
    sprite.setAnchor(ngf::Anchor::Center);
    sprite.draw(target, localStates);
  }
};