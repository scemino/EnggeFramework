#pragma once
#include <ngf/Graphics/Drawable.h>
#include <ngf/IO/Json/JsonParser.h>
#include <ngf/Math/PathFinding/Walkbox.h>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include "SpriteSheetItem.h"
#include "Camera.h"
#include "Layer.h"
#include "Light.h"
#include "LightingShader.h"
#include "Object.h"

struct ScalingValue {
  float scale;
  int y;
};

struct Scaling {
  std::vector<ScalingValue> values;
  std::string trigger;
};

class Layers final {
public:
  using iterator = std::vector<std::unique_ptr<Layer>>::iterator;
  using const_iterator = std::vector<std::unique_ptr<Layer>>::const_iterator;

  explicit Layers(std::vector<std::unique_ptr<Layer>> &layers) : m_layers(layers) {
  }

  iterator begin() noexcept { return m_layers.begin(); }
  iterator end() noexcept { return m_layers.end(); }
  [[nodiscard]] const_iterator cbegin() const noexcept { return m_layers.cbegin(); }
  [[nodiscard]] const_iterator cend() const noexcept { return m_layers.cend(); }

private:
  std::vector<std::unique_ptr<Layer>> &m_layers;
};

class Walkboxes final {
public:
  using iterator = std::vector<ngf::Walkbox>::iterator;
  using const_iterator = std::vector<ngf::Walkbox>::const_iterator;
  using const_reference = std::vector<ngf::Walkbox>::const_reference;

  explicit Walkboxes(std::vector<ngf::Walkbox> &walkboxes) : m_walkboxes(walkboxes) {
  }

  iterator begin() noexcept { return m_walkboxes.begin(); }
  iterator end() noexcept { return m_walkboxes.end(); }
  [[nodiscard]] const_iterator cbegin() const noexcept { return m_walkboxes.cbegin(); }
  [[nodiscard]] const_iterator cend() const noexcept { return m_walkboxes.cend(); }

  void push_back(const_reference walkbox) { m_walkboxes.push_back(walkbox); }

  iterator erase(const_iterator position) { return m_walkboxes.erase(position); }

private:
  std::vector<ngf::Walkbox> &m_walkboxes;
};

class Frames final {
public:
  using iterator = std::map<std::string, SpriteSheetItem>::iterator;
  using const_iterator = std::map<std::string, SpriteSheetItem>::const_iterator;

  explicit Frames(std::map<std::string, SpriteSheetItem> &items) : m_items(items) {
  }

  iterator begin() noexcept { return m_items.begin(); }
  iterator end() noexcept { return m_items.end(); }
  [[nodiscard]] const_iterator cbegin() const noexcept { return m_items.cbegin(); }
  [[nodiscard]] const_iterator cend() const noexcept { return m_items.cend(); }

private:
  std::map<std::string, SpriteSheetItem> &m_items;
};

class Objects final {
public:
  using iterator = std::vector<Object>::iterator;
  using const_iterator = std::vector<Object>::const_iterator;
  using const_reference = std::vector<Object>::const_reference;
  using reference = std::vector<Object>::reference;

  explicit Objects(std::vector<Object> &objects) : m_objects(objects) {
  }

  iterator begin() noexcept { return m_objects.begin(); }
  iterator end() noexcept { return m_objects.end(); }
  [[nodiscard]] const_iterator cbegin() const noexcept { return m_objects.cbegin(); }
  [[nodiscard]] const_iterator cend() const noexcept { return m_objects.cend(); }

  void push_back(const_reference object) { m_objects.push_back(object); }
  reference back() noexcept { return m_objects.back(); }
  iterator erase(const_iterator position) { return m_objects.erase(position); }

private:
  std::vector<Object> &m_objects;
};

class Room final : public ngf::Drawable {
public:
  ~Room() override = default;

  glm::ivec2 getScreenSize() const noexcept {
    return getScreenSize(m_height);
  }

  constexpr bool isLoaded() const { return m_isLoaded; }
  std::shared_ptr<ngf::Texture> getTexture() { return m_texture; }

  void loadRoom(const std::filesystem::path &path) {
    if(!m_shader) {
      m_shader = std::make_shared<LightingShader>();
    }

    m_path = path;
    m_layers.clear();
    m_objects.clear();
    m_scalings.clear();
    m_walkboxes.clear();
    m_frames.clear();

    // load wimpy
    const auto wimpy = ngf::Json::load(path);
    m_name = wimpy["name"].getString();
    m_sheet = wimpy["sheet"].getString();

    // load sheet
    auto sheetPath = path.parent_path().parent_path();
    sheetPath.append("SpriteSheets");
    sheetPath.append(m_sheet).replace_extension(".json");
    loadSheet(sheetPath);

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
          items.push_back(m_frames[name]);
        }
      } else {
        auto name = wimpy["background"].getString();
        items.push_back(m_frames[name]);
      }
      auto background = std::make_unique<Layer>(offsetY, m_roomSize.y, m_texture, items, glm::vec2{1.f, 1.f}, 0.f);
      m_layers.push_back(std::move(background));
    }

    // layers
    if (!wimpy["layers"].isNull()) {
      for (const auto &gLayer : wimpy["layers"]) {
        std::vector<SpriteSheetItem> items;
        auto parallax = parseParallax(gLayer["parallax"]);
        auto zsort = gLayer["zsort"].getDouble();
        if (gLayer["name"].isArray()) {
          for (const auto &name : gLayer["name"]) {
            items.push_back(m_frames[name.getString()]);
          }
        } else if (gLayer["name"].isString()) {
          items.push_back(m_frames[gLayer["name"].getString()]);
        }
        auto layer = std::make_unique<Layer>(offsetY, m_roomSize.y, m_texture, items, parallax, zsort);
        m_layers.push_back(std::move(layer));
      }
    }

    // sort layers by zsort
    std::sort(m_layers.begin(), m_layers.end(), [](const auto &l1, const auto &l2) {
      return l1->getZSort() > l2->getZSort();
    });

    loadWalkboxes(wimpy);
    loadObjects(wimpy);
    loadScalings(wimpy);

    // lights
    lights[0].color = ngf::Color(0xff, 0x00, 0xa2);
    lights[0].pos = {256, 238};
    lights[0].brightness = 2.0f;
    lights[0].coneDirection = 180.f;
    lights[0].coneAngle = 30.f;
    lights[0].coneFalloff = 0.15f;
    lights[0].cutOffRadius = 250.f;
    lights[0].halfRadius = 0.25f;

//    lights[0].color = ngf::Color(0xb6, 0xe3, 0xdb);
//    lights[0].pos = {130, 178};
//    lights[0].brightness = 0.5f;
//    lights[0].coneDirection = 180.f;
//    lights[0].coneAngle = 150.f;
//    lights[0].coneFalloff = 0.15f;
//    lights[0].cutOffRadius = 200.f;
//    lights[0].halfRadius = 0.75f;

//    lights[1].color = ngf::Color(0xb6, 0xe3, 0xdb);
//    lights[1].pos = {300, 178};
//    lights[1].brightness = 0.5f;
//    lights[1].coneDirection = 180.f;
//    lights[1].coneAngle = 130.f;
//    lights[1].coneFalloff = 0.15f;
//    lights[1].cutOffRadius = 200.f;
//    lights[1].halfRadius = 0.75f;
//    numLights = 1;

    m_camera.position = glm::vec2(0, 0);
    m_camera.size = getScreenSize(m_height);
    m_isLoaded = true;
  }

  void save() {
    ngf::GGPackValue gRoom;

    // background
    auto it = std::find_if(m_layers.cbegin(), m_layers.cend(), [](const auto &layer) {
      return layer->getZSort() == 0;
    });
    const auto &items = it->get()->getItems();
    if (items.size() == 1) {
      gRoom["background"] = items[0].name;
    } else {
      for (const auto &item : items) {
        gRoom["background"].push_back(item.name);
      }
    }
    // fullscreen
    gRoom["fullscreen"] = m_fullscreen;
    // height
    gRoom["height"] = m_height;
    // layers
    for (const auto &layer : m_layers) {
      if (layer->getZSort() == 0)
        continue;
      ngf::GGPackValue gLayer;
      const auto &names = layer->getItems();
      if (names.size() == 1) {
        gLayer["name"] = names[0].name;
      } else {
        for (const auto &name : names) {
          gLayer["name"].push_back(name.name);
        }
      }
      auto parallax = layer->getParallax();
      if (parallax.y == 1) {
        gLayer["parallax"] = parallax.x;
      } else {
        gLayer["parallax"] = toString(parallax);
      }
      gLayer["zsort"] = layer->getZSort();
      gRoom["layers"].push_back(gLayer);
    }
    // name
    gRoom["name"] = m_name;
    // objects
    for (const auto &object : m_objects) {
      ngf::GGPackValue gObj;
      for (const auto &anim : object.animations) {
        auto gAnim = toGGPackValue(anim);
        gObj["animations"].push_back(gAnim);
      }
      gObj["hotspot"] = toString(object.hotspot);
      gObj["name"] = object.name;
      gObj["pos"] = toString(object.pos);
      switch (object.type) {
      case ObjectType::Prop:gObj["prop"] = 1;
        break;
      case ObjectType::Spot:gObj["spot"] = 1;
        break;
      case ObjectType::Trigger:gObj["trigger"] = 1;
        break;
      default:break;
      }
      switch (object.useDir) {
      case Direction::Back:gObj["usedir"] = "DIR_BACK";
        break;
      case Direction::Front:gObj["usedir"] = "DIR_FRONT";
        break;
      case Direction::Left:gObj["usedir"] = "DIR_LEFT";
        break;
      case Direction::Right:gObj["usedir"] = "DIR_RIGHT";
        break;
      default:break;
      }
      gObj["usepos"] = toString(object.usePos);
      gObj["zsort"] = object.zsort;
      gRoom["objects"].push_back(gObj);
    }
    // roomSize
    gRoom["roomsize"] = toString(m_roomSize);
    // scalings
    if (m_scalings.size() == 1) {
      for (auto &value : m_scalings[0].values) {
        gRoom["scaling"].push_back(toString(value));
      }
    } else {
      for (const auto &scaling : m_scalings) {
        ngf::GGPackValue gScaling;
        for (auto &value : scaling.values) {
          gScaling["scaling"].push_back(toString(value));
        }
        if (!scaling.trigger.empty())
          gScaling["trigger"] = scaling.trigger;
        gRoom["scaling"].push_back(gScaling);
      }
    }
    // sheet
    gRoom["sheet"] = m_sheet;
    // walkboxes
    for (auto &walkbox : m_walkboxes) {
      ngf::GGPackValue gWalkbox;
      if (!walkbox.getName().empty()) {
        gWalkbox["name"] = walkbox.getName();
      }
      std::ostringstream s;
      for (const auto &pos : walkbox) {
        s << toString(pos) << ';';
      }
      gWalkbox["polygon"] = s.str().substr(0, s.str().size() - 1);
      gRoom["walkboxes"].push_back(gWalkbox);
    }

    auto content = gRoom.toString();
    std::ofstream os(m_path);
    os << content;
    os.close();
  }

  static ngf::GGPackValue toGGPackValue(const ObjectAnimation &anim) {
    ngf::GGPackValue gAnim;
    // HACK: to save empty frames
    if (anim.frames.empty() && anim.layers.empty()) {
      gAnim["frames"].push_back("");
      gAnim["frames"].clear();
    }
    gAnim["name"] = anim.name;
    for (const auto &frame : anim.frames) {
      gAnim["frames"].push_back(frame.name);
    }
    for (const auto &layer : anim.layers) {
      auto gLayer = toGGPackValue(layer);
      gAnim["layers"].push_back(gLayer);
    }
    if (anim.fps)
      gAnim["fps"] = anim.fps;
    if (anim.loop)
      gAnim["loop"] = 1;
    return gAnim;
  }

  static std::string toString(const ScalingValue &value) {
    std::ostringstream s;
    s << value.scale << '@' << value.y;
    return s.str();
  }

  static std::string toString(glm::ivec2 pos) {
    std::ostringstream s;
    s << '{' << pos.x << ',' << pos.y << '}';
    return s.str();
  }

  static std::string toString(glm::vec2 pos) {
    std::ostringstream s;
    s << '{' << pos.x << ',' << pos.y << '}';
    return s.str();
  }

  static std::string toString(ngf::irect rect) {
    std::ostringstream s;
    s << "{{" << rect.min.x << ',' << rect.min.y << "},{" << rect.max.x << ',' << rect.max.y << "}}";
    return s.str();
  }

  void update(const ngf::TimeSpan &elapsed) {
    for (auto &obj : m_objects) {
      obj.update(elapsed);
    }
  }

  void draw(ngf::RenderTarget &target, ngf::RenderStates states) const override {

    ngf::View view{ngf::frect::fromPositionSize(glm::ivec2(), m_camera.size)};
    view.zoom(m_camera.zoom);
    target.setView(view);

    // update lighting
    auto nLights = numLights;
    m_shader->setAmbientColor(ambientLight);
    m_shader->setNumberLights(numLights);
    m_shader->setLights(lights);

    // draw background layers
    for (const auto &layer : m_layers) {
      if (layer->getZSort() < 0)
        continue;

      ngf::RenderStates layerStates = states;
      layerStates.shader = m_shader.get();
      ngf::Transform t;
      t.setPosition({-m_camera.position.x * layer->getParallax().x, m_camera.position.y * layer->getParallax().y});
      layerStates.transform *= t.getTransform();
      layer->draw(target, layerStates);
    }

    // draw objects
    ngf::RenderStates localStates = states;
    localStates.shader = m_shader.get();
    ngf::Transform t;
    t.setPosition({-m_camera.position.x, m_camera.position.y});
    localStates.transform *= t.getTransform();

    if (m_objectsZOrderDirty) {
      m_objectsZOrderDirty = false;
      std::sort(m_objects.begin(), m_objects.end(), [](const auto &o1, const auto &o2) {
        return o1.zsort > o2.zsort;
      });
    }

    for (const auto &object : m_objects) {
      object.draw(target, localStates);
    }

    // draw foreground layers
    for (const auto &layer : m_layers) {
      if (layer->getZSort() >= 0)
        continue;
      ngf::Transform t;
      t.setPosition({-m_camera.position.x * layer->getParallax().x, m_camera.position.y * layer->getParallax().y});
      ngf::RenderStates layerStates = states;
      layerStates.shader = m_shader.get();
      layerStates.transform *= t.getTransform();
      layer->draw(target, layerStates);
    }
  }

  int getHeight() const noexcept { return m_height; }

  glm::ivec2 getSize() const noexcept { return m_roomSize; }

  Camera &getCamera() noexcept { return m_camera; }
  const Camera &getCamera() const noexcept { return m_camera; }

  Layers layers() noexcept { return Layers(m_layers); }
  Objects objects() noexcept { return Objects(m_objects); }
  Walkboxes walkboxes() noexcept { return Walkboxes(m_walkboxes); }
  Frames frames() noexcept { return Frames(m_frames); }

private:
  void loadSheet(const std::filesystem::path &path) {
    auto sheetJson = ngf::Json::load(path);
    const auto &frames = sheetJson["frames"];
    for (const auto &frame : frames.items()) {
      auto sheetItem = toSpriteSheetItem(frame.key(), frame.value());
      m_frames.insert({frame.key(), sheetItem});
    }
  }

  void loadWalkboxes(const ngf::GGPackValue &wimpy) {
    for (const auto &gWalkbox : wimpy["walkboxes"]) {
      auto walkbox = parsePolygon(gWalkbox["polygon"].getString());
      walkbox.setYAxisDirection(ngf::YAxisDirection::Up);
      if (gWalkbox["name"].isString()) {
        walkbox.setName(gWalkbox["name"].getString());
      }
      m_walkboxes.push_back(walkbox);
    }
  }

  void loadObjects(const ngf::GGPackValue &wimpy) {
    for (const auto &gObject : wimpy["objects"]) {
      auto object = parseObject(gObject);
      m_objects.push_back(object);
    }
  }

  void loadScalings(const ngf::GGPackValue &wimpy) {
    const auto wScalings = wimpy["scaling"];
    if (wScalings.size() == 0)
      return;

    if (wScalings[0].isString()) {
      m_scalings.push_back(loadScaling(wScalings));
      return;
    }

    for (const auto &wScaling : wScalings) {
      auto scaling = loadScaling(wScaling["scaling"]);
      if (!wScaling["trigger"].isNull()) {
        scaling.trigger = wScaling["trigger"].getString();
      }
      m_scalings.push_back(scaling);
    }
  }

  static Scaling loadScaling(const ngf::GGPackValue &scaling) {
    Scaling s;
    for (const auto &value : scaling) {
      auto v = value.getString();
      char *end;
      auto scale = std::strtof(v.c_str(), &end);
      auto y = std::strtol(end + 1, &end, 10);
      s.values.push_back({scale, static_cast<int>(y)});
    }
    return s;
  }

  static glm::ivec2 getScreenSize(int roomHeight) {
    switch (roomHeight) {
    case 128:return glm::ivec2(320, 180);
    case 172:return glm::ivec2(428, 240);
    case 256:return glm::ivec2(640, 360);
    default: {
      assert(false);
    }
    }
    return {};
  }

  Object parseObject(const ngf::GGPackValue &gObject) {
    Object obj;
    obj.room = this;
    obj.texture = m_texture;
    obj.name = gObject["name"].getString();
    obj.pos = parseIVec2(gObject["pos"].getString());
    obj.usePos = parseIVec2(gObject["usepos"].getString());
    obj.useDir = parseUseDir(gObject["usedir"].getString());
    obj.hotspot = parseIRect(gObject["hotspot"].getString());
    obj.zsort = gObject["zsort"].getInt();
    obj.type = toObjectType(gObject);
    auto anims = parseObjectAnimations(gObject["animations"]);
    obj.animations = std::move(anims);
    return obj;
  }

  std::vector<ObjectAnimation> parseObjectAnimations(const ngf::GGPackValue &gAnimations) {
    std::vector<ObjectAnimation> anims;
    if (gAnimations.isNull())
      return anims;
    for (const auto &gAnimation : gAnimations) {
      if (gAnimation.isNull())
        continue;
      anims.push_back(parseObjectAnimation(gAnimation));
    }
    return anims;
  }

  ObjectAnimation parseObjectAnimation(const ngf::GGPackValue &gAnimation) {
    ObjectAnimation anim;
    anim.name = gAnimation["name"].getString();
    anim.loop = toBool(gAnimation["loop"]);
    anim.fps = gAnimation["fps"].isNull() ? 0.f : gAnimation["fps"].getDouble();
    anim.flags = gAnimation["flags"].isNull() ? 0 : gAnimation["flags"].getInt();
    if (!gAnimation["frames"].isNull()) {
      for (const auto &gFrame : gAnimation["frames"]) {
        auto name = gFrame.getString();
        anim.frames.push_back(m_frames[name]);
      }
      return anim;
    }

    if (!gAnimation["layers"].isNull()) {
      for (const auto &gLayer : gAnimation["layers"]) {
        auto layer = parseObjectAnimation(gLayer);
        anim.layers.push_back(layer);
      }
    }
    return anim;
  }

  static ObjectType toObjectType(const ngf::GGPackValue &gObject) {
    if (toBool(gObject["prop"])) {
      return ObjectType::Prop;
    }
    if (toBool(gObject["spot"])) {
      return ObjectType::Spot;
    }
    if (toBool(gObject["trigger"])) {
      return ObjectType::Trigger;
    }
    return ObjectType::None;
  }

  static Direction parseUseDir(std::string_view text) {
    if (text == "DIR_FRONT")
      return Direction::Front;
    if (text == "DIR_BACK")
      return Direction::Back;
    if (text == "DIR_LEFT")
      return Direction::Left;
    if (text == "DIR_RIGHT")
      return Direction::Right;
    return Direction::Back;
  }

  static glm::vec2 parseParallax(const ngf::GGPackValue &gValue) {
    if (gValue.isDouble()) {
      return glm::vec2{gValue.getDouble(), 1.f};
    }

    if (gValue.isString()) {
      auto str = gValue.getString();
      char *end;
      auto x = std::strtof(str.data() + 1, &end);
      auto y = std::strtof(end + 1, &end);
      return glm::vec2{x, y};
    }
    assert(false);
    return glm::ivec2{};
  }

  static bool toBool(const ngf::GGPackValue &gValue) {
    return !gValue.isNull() && gValue.getInt() == 1;
  }

  static ngf::Walkbox parsePolygon(std::string_view text) {
    const char *p = text.data();
    auto size = text.size();
    char *p2;
    std::vector<glm::ivec2> points;
    do {
      auto x = std::strtol(p + 1, &p2, 10);
      auto y = std::strtol(p2 + 1, &p2, 10);
      points.emplace_back(x, y);
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

  static ngf::irect parseIRect(std::string_view value) {
    char *ptr;
    auto x = std::strtol(value.data() + 2, &ptr, 10);
    auto y = std::strtol(ptr + 1, &ptr, 10);
    auto x2 = std::strtol(ptr + 3, &ptr, 10);
    auto y2 = std::strtol(ptr + 1, &ptr, 10);
    return ngf::irect::fromMinMax({x, y}, {x2, y2});
  }

public:
  ngf::Color ambientLight{ngf::Colors::White};
  int numLights{0};
  std::array<Light, 50> lights;

private:
  std::vector<std::unique_ptr<Layer>> m_layers;
  std::shared_ptr<ngf::Texture> m_texture;
  std::vector<ngf::Walkbox> m_walkboxes;
  std::vector<Scaling> m_scalings;
  mutable std::vector<Object> m_objects;
  glm::ivec2 m_roomSize{0, 0};
  int m_fullscreen{0};
  int m_height{0};
  Camera m_camera{};
  std::map<std::string, SpriteSheetItem> m_frames;
  mutable bool m_objectsZOrderDirty{true};
  std::string m_sheet;
  bool m_isLoaded{false};
  std::string m_name;
  std::filesystem::path m_path;
  std::shared_ptr<LightingShader> m_shader;
};