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
  std::vector<ObjectAnimation> layers;
  bool loop{false};
  double fps{0.f};
  int flags{0};
};

class Room;
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
  Room *room{nullptr};

public:
  ~Object() override = default;

  void play();
  void pause();

  void update(const ngf::TimeSpan &e);
  void draw(ngf::RenderTarget &target, ngf::RenderStates states) const override;

private:
  AnimState m_state{AnimState::Pause};
};