#pragma once
#include <ngf/Graphics/Drawable.h>
#include <ngf/IO/Json/JsonParser.h>
#include <ngf/Math/PathFinding/Walkbox.h>
#include <algorithm>
#include <filesystem>
#include "SpriteSheetItem.h"
#include "Camera.h"
#include "Layer.h"

class Layers {
public:
  using iterator = std::vector<std::unique_ptr<Layer>>::iterator;
  using const_iterator = std::vector<std::unique_ptr<Layer>>::const_iterator;

  explicit Layers(std::vector<std::unique_ptr<Layer>> &layers) : m_layers(layers) {
  }

  iterator begin() noexcept { return m_layers.begin(); }
  iterator end() noexcept { return m_layers.end(); }
  const_iterator cbegin() const noexcept { return m_layers.cbegin(); }
  const_iterator cend() const noexcept { return m_layers.cend(); }

private:
  std::vector<std::unique_ptr<Layer>> &m_layers;
};

class Walkboxes {
public:
  using iterator = std::vector<ngf::Walkbox>::iterator;
  using const_iterator = std::vector<ngf::Walkbox>::const_iterator;

  explicit Walkboxes(std::vector<ngf::Walkbox> &walkboxes) : m_walkboxes(walkboxes) {
  }

  iterator begin() noexcept { return m_walkboxes.begin(); }
  iterator end() noexcept { return m_walkboxes.end(); }
  const_iterator cbegin() const noexcept { return m_walkboxes.cbegin(); }
  const_iterator cend() const noexcept { return m_walkboxes.cend(); }

private:
  std::vector<ngf::Walkbox> &m_walkboxes;
};

class Room final : public ngf::Drawable {
public:
  ~Room() override = default;

  void loadRoom(const std::filesystem::path &path) {
    // load wimpy
    const auto wimpy = ngf::Json::load(path);
    auto sheet = wimpy["sheet"].getString();

    // load sheet
    auto sheetPath = path;
    sheetPath.replace_filename(sheet).replace_extension(".json");
    const auto sheetJson = ngf::Json::load(sheetPath);

    // load texture
    auto image = sheetPath;
    image.replace_extension(".png");
    m_texture = std::make_shared<ngf::Texture>(image);

    m_height = wimpy["height"].getInt();
    m_roomSize = parseIVec2(wimpy["roomsize"].getString());
    const auto screenSize = getScreenSize(m_height);
    const int offsetY = screenSize.y - m_roomSize.y;
    m_fullscreen = wimpy["fullscreen"].getInt();

    // background
    {
      std::vector<SpriteSheetItem> items;
      if (wimpy["background"].isArray()) {
        for (const auto &background : wimpy["background"]) {
          auto name = background.getString();
          items.push_back(toSpriteSheetItem(name, sheetJson["frames"][name]));
        }
      } else {
        auto name = wimpy["background"].getString();
        items.push_back(toSpriteSheetItem(name, sheetJson["frames"][name]));
      }
      auto background = std::make_unique<Layer>(offsetY, m_roomSize.y, m_texture, items, glm::vec2{1.f, 1.f}, 0.f);
      m_layers.push_back(std::move(background));
    }

    // layers
    for (const auto &gLayer : wimpy["layers"]) {
      std::vector<SpriteSheetItem> items;
      auto parallax = gLayer["parallax"].getDouble();
      auto zsort = gLayer["zsort"].getDouble();
      if (gLayer["name"].isArray()) {
        for (const auto &name : gLayer["name"]) {
          items.push_back(toSpriteSheetItem(name.getString(), sheetJson["frames"][name.getString()]));
        }
      } else if (gLayer["name"].isString()) {
        items.push_back(toSpriteSheetItem(gLayer["name"].getString(), sheetJson["frames"][gLayer["name"].getString()]));
      }
      auto layer = std::make_unique<Layer>(offsetY, m_roomSize.y, m_texture, items, glm::vec2{parallax, 1.f}, zsort);
      m_layers.push_back(std::move(layer));
    }

    // sort layers by zsort
    std::sort(m_layers.begin(), m_layers.end(), [](const auto &l1, const auto &l2) {
      return l1->getZSort() > l2->getZSort();
    });

    loadWalkboxes(wimpy);

    m_camera.position = glm::vec2(0, m_roomSize.y / 2.f);
    m_camera.size = getScreenSize(m_height);
  }

  void loadWalkboxes(const ngf::GGPackValue &wimpy) {
    for (const auto &gWalkbox : wimpy["walkboxes"]) {
      auto walkbox = parsePolygon(gWalkbox["polygon"].getString());
      if (gWalkbox["name"].isString()) {
        walkbox.setName(gWalkbox["name"].getString());
      }
      m_walkboxes.push_back(walkbox);
    }
  }

  ngf::Transform &getTransform() { return m_transform; }

  void draw(ngf::RenderTarget &target, ngf::RenderStates states) const override {
    states.transform *= m_transform.getTransform();

    for (const auto &layer : m_layers) {
      ngf::RenderStates layerStates = states;
      ngf::Transform t;
      t.setPosition({(-m_camera.position.x) * layer->getParallax().x, 0});
      layerStates.transform *= t.getTransform();
      layer->draw(target, layerStates);
    }

    for (const auto &walkbox : m_walkboxes) {
      if (!walkbox.isVisible())
        continue;

      ngf::RenderStates localStates = states;
      ngf::Transform t;
      t.setPosition({-m_camera.position.x, 0});
      localStates.transform *= t.getTransform();

      std::vector<ngf::Vertex> polygon;
      std::transform(walkbox.cbegin(), walkbox.cend(), std::back_inserter(polygon), [](const auto &p) {
        return ngf::Vertex{(glm::vec2) p, ngf::Colors::LimeGreen};
      });
      target.draw(ngf::PrimitiveType::LineLoop, polygon, localStates);
    }
  }

  glm::ivec2 getSize() const { return m_roomSize; }

  Camera &getCamera() { return m_camera; }
  const Camera &getCamera() const { return m_camera; }

  Layers layers() noexcept { return Layers(m_layers); }
  Walkboxes walkboxes() noexcept { return Walkboxes(m_walkboxes); }

private:
  static glm::ivec2 getScreenSize(int roomHeight) {
    switch (roomHeight) {
    case 128:return glm::ivec2(320, 180);
    case 172:return glm::ivec2(428, 240);
    case 256:return glm::ivec2(640, 360);
    default: {
      assert(false);
    }
    }
  }

  ngf::Walkbox parsePolygon(std::string_view text) const {
    const char *p = text.data();
    auto size = text.size();
    char *p2;
    std::vector<glm::ivec2> points;
    do {
      auto x = std::strtol(p + 1, &p2, 10);
      auto y = std::strtol(p2 + 1, &p2, 10);
      points.emplace_back(x, m_roomSize.y - y);
      p = p2 + 2;
    } while ((p - text.data()) < size);
    return ngf::Walkbox(points);
  }

  static SpriteSheetItem toSpriteSheetItem(const std::string &name, const ngf::GGPackValue &value) {
    auto rect = toRect(value["frame"]);
    auto spriteSourceSize = toRect(value["spriteSourceSize"]);
    auto sourceSize = toSize(value["sourceSize"]);
    return SpriteSheetItem{name, rect, spriteSourceSize, sourceSize};
  }

  static ngf::irect toRect(const ngf::GGPackValue &value) {
    auto x = value["x"].getInt();
    auto y = value["y"].getInt();
    auto w = value["w"].getInt();
    auto h = value["h"].getInt();
    return ngf::irect::fromPositionSize({x, y}, {w, h});
  }

  static glm::ivec2 toSize(const ngf::GGPackValue &value) {
    auto w = value["w"].getInt();
    auto h = value["h"].getInt();
    return glm::ivec2({w, h});
  }

  static glm::ivec2 parseIVec2(std::string_view value) {
    char *ptr;
    auto x = std::strtol(value.data() + 1, &ptr, 10);
    auto y = std::strtol(ptr + 1, &ptr, 10);
    return glm::ivec2{x, y};
  }

private:
  std::vector<std::unique_ptr<Layer>> m_layers;
  std::shared_ptr<ngf::Texture> m_texture;
  std::vector<ngf::Walkbox> m_walkboxes;
  ngf::Transform m_transform;
  glm::ivec2 m_roomSize{0, 0};
  int m_fullscreen{0};
  int m_height{0};
  Camera m_camera{};
};