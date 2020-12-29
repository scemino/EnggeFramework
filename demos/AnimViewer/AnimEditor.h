#pragma once
#include <memory>
#include <ngf/Graphics/Sprite.h>
#include <ngf/Math/Transform.h>
#include <ngf/IO/Json/JsonParser.h>
#include "SpriteSheetItem.h"
#include "Object.h"
#include "Camera.h"
#include <ngf/Graphics/ImGuiExtensions.h>

class AnimEditor final {
public:
  void loadAnim(const std::filesystem::path &path) {
    m_selectedAnim = nullptr;
    auto gAnims = ngf::Json::load(path);
    auto sheet = gAnims["sheet"].getString() + ".png";
    auto sheetPath = path;
    sheetPath = sheetPath.parent_path().append(sheet);

    m_texture = std::make_shared<ngf::Texture>(sheetPath);
    sheetPath = sheetPath.replace_extension(".json");
    loadSheet(sheetPath);

    m_animations = parseObjectAnimations(gAnims["animations"]);
  }

  void onRender(ngf::RenderTarget &target) {
    ngf::View view{ngf::frect::fromCenterSize(glm::ivec2(160, 90), {320, 180})};
    view.zoom(m_camera.zoom);
    target.setView(view);

    ngf::RenderStates s;
    ngf::Transform t;
    t.setPosition({-m_camera.position.x, m_camera.position.y});
    s.transform *= t.getTransform();

    target.clear(ngf::Colors::AliceBlue);
    drawAnim(target, s);
  }

  void update(const ngf::TimeSpan &e) {
    if (m_state == State::Pause)
      return;

    if (!m_selectedAnim)
      return;

    auto &animation = *m_selectedAnim;
    if (animation.frames.empty() && animation.layers.empty())
      return;

    if (!animation.frames.empty()) {
      update(e, animation);
      return;
    }

    for (auto &layer : animation.layers) {
      update(e, layer);
    }
  }

  void draw() {
    showAnimationFrames();
    showAnimationList();
    showAnimation();
  }

private:
  static void update(const ngf::TimeSpan &e, ObjectAnimation &animation) {
    if (animation.frameIndex == -1) {
      animation.frameIndex = animation.frames.size() - 1;
    }

    if (animation.frameIndex > animation.frames.size()) {
      animation.frameIndex = 0;
      return;
    }

    animation.elapsed += e;
    auto fps = animation.fps;
    if (fps == 0)
      fps = 10;
    const auto frameTime = 1.f / static_cast<float>(fps);
    if (animation.elapsed.getTotalSeconds() > frameTime) {
      animation.elapsed = ngf::TimeSpan::seconds(animation.elapsed.getTotalSeconds() - frameTime);
      animation.frameIndex = (animation.frameIndex + 1) % animation.frames.size();
    }
  }

  void drawAnim(ngf::RenderTarget &target, ngf::RenderStates states) {
    if (!m_selectedAnim)
      return;
    if (m_selectedAnim->frames.empty() && m_selectedAnim->layers.empty())
      return;

    ngf::Transform t;
    t.setPosition({160, 90});
    t.setScale({m_scale, m_scale});
    states.transform *= t.getTransform();

    draw(*m_selectedAnim, target, states);

    for (const auto &layer : m_selectedAnim->layers) {
      draw(layer, target, states);
    }
  }

  void draw(const ObjectAnimation &anim, ngf::RenderTarget &target, ngf::RenderStates states) {
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
        m_left ?
        frame.sourceSize.x - frame.spriteSourceSize.min.x - offset.x :
        frame.spriteSourceSize.min.x + offset.x,
        frame.spriteSourceSize.min.y - offset.y};

    ngf::Transform tFlipX;
    tFlipX.setScale({m_left ? -1 : 1, 1});
    ngf::Transform t;
    t.setOrigin(origin);
    t.setPosition(pos);
    states.transform = tFlipX.getTransform() * t.getTransform() * states.transform;

    ngf::Sprite sprite(*m_texture, frame.frame);
    sprite.draw(target, states);
  }

  void showAnimationList() {
    ImGui::Begin("Animation List");

    m_filterAnim.Draw();

    // count the number of objects not filtered
    int countObjects = 0;
    for (const auto &anim : m_animations) {
      if (!m_filterAnim.PassFilter(anim.name.c_str()))
        continue;
      countObjects++;
    }

    // display them
    ImGui::Columns(3, nullptr, true);
    auto maxByColumn = std::max(countObjects / 3, 1);
    int count = 0;
    for (auto &anim : m_animations) {
      if (!m_filterAnim.PassFilter(anim.name.c_str()))
        continue;

      if (count++ >= maxByColumn) {
        ImGui::NextColumn();
        count = 1;
      }

      ImGui::PushID(&anim);
      bool isSelected = &anim == m_selectedAnim;
      if (ImGui::Selectable(anim.name.c_str(), isSelected)) {
        setSelectAnim(&anim);
      }
      ImGui::PopID();
    }
    ImGui::Columns(1);

    ImGui::End();
  }

  void showAnimationFrames() {
    ImGui::Begin("Animation Frames");
    ImGui::Separator();
    HelpMarker("Drag to Animation States window to add frames.");
    ImGui::Separator();
    for (const auto &frame : m_frames) {
      ImGui::Button(frame.first.c_str());

      if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("DND_ANIM_FRAME", &frame.second, sizeof(SpriteSheetItem));
        ImGui::Text("%s", frame.first.c_str());
        ImGui::EndDragDropSource();
      }
    }

    ImGui::End();
  }

  void showAnimation() {
    ImGui::Begin("Animation States");
    ImGui::DragFloat2("Camera Position", &m_camera.position.x);
    ImGui::DragFloat("Camera Zoom", &m_camera.zoom);
    ImGui::Checkbox("Left", &m_left);
    ImGui::DragFloat("Scale", &m_scale);
    ImGui::Separator();
    if (m_selectedAnim) {
      ImGui::Text("Animation name : %s", m_selectedAnim->name.c_str());
      ImGui::Separator();

      HelpMarker("Drag to rearrange frames.\nRight-click to delete.");
      ImGui::Separator();

      std::string control = m_state == State::Play ? "Play" : "Pause";
      if (ImGui::ArrowButton(control.c_str(), ImGuiDir_Right)) {
        m_state = m_state == State::Play ? State::Pause : State::Play;
      }
      ImGui::Separator();
      if (ImGui::TreeNode("frames")) {
        ImGui::DragInt("fps", &m_selectedAnim->fps);
        ImGui::Checkbox("loop", &m_selectedAnim->loop);
        ImGui::DragInt("flags", &m_selectedAnim->flags);
        showFrames(m_selectedAnim->frames);
        ImGui::TreePop();
      }
      if (ImGui::TreeNode("layers")) {
        for (int i = 0; i < m_selectedAnim->layers.size(); i++) {
          auto &layer = m_selectedAnim->layers[i];
          ImGui::PushID(&layer);
          auto expanded = ImGui::TreeNode(layer.name.c_str());
          ImGui::SameLine();
          ImGui::Checkbox("", &layer.visible);
          ImGui::PopID();
          if (expanded) {
            ImGui::DragInt("fps", &layer.fps);
            ImGui::Checkbox("loop", &layer.loop);
            ImGui::DragInt("flags", &layer.flags);
            showFrames(layer.frames);
            ImGui::Separator();
            if (layer.frames.size() > 1) {
              ImGui::SliderInt("Frame", &layer.frameIndex, 0, layer.frames.size() - 1);
            }
            ImGui::TreePop();
          }
        }
        ImGui::TreePop();
      }
    }
    ImGui::End();
  }

  static void showFrames(std::vector<SpriteSheetItem> &items) {
    for (int i = 0; i < items.size(); i++) {
      const auto &frame = items[i];
      ImGui::Button(frame.name.c_str());
      if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("DND_ANIM_FRAME",
                                  &frame,
                                  sizeof(SpriteSheetItem));    // Set payload to carry the index of our item (could be anything)
        ImGui::Text("%s", frame.name.c_str());
        ImGui::EndDragDropSource();
      }
      if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_ANIM_FRAME")) {
          IM_ASSERT(payload->DataSize == sizeof(SpriteSheetItem));
          const auto &payload_n = *(const SpriteSheetItem *) payload->Data;
          auto it = std::find_if(items.cbegin(),
                                 items.cend(),
                                 [&payload_n](const auto &item) {
                                   return item.name == payload_n.name;
                                 });
          //setModified();
          if (it != items.cend()) {
            auto tmp = items[i];
            auto n = it - items.begin();
            items[i] = items[n];
            items[n] = tmp;
          } else {
            items.insert(items.begin() + i, payload_n);
          }
        }
        ImGui::EndDragDropTarget();
      }
    }
  }

  void setSelectAnim(ObjectAnimation *pAnim) {
    m_selectedAnim = pAnim;
  }

  static void HelpMarker(const char *desc) {
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
      ImGui::BeginTooltip();
      ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
      ImGui::TextUnformatted(desc);
      ImGui::PopTextWrapPos();
      ImGui::EndTooltip();
    }
  }

  void loadSheet(const std::filesystem::path &path) {
    m_frames.clear();
    auto sheetJson = ngf::Json::load(path);
    const auto &frames = sheetJson["frames"];
    for (const auto &frame : frames.items()) {
      auto sheetItem = toSpriteSheetItem(frame.key(), frame.value());
      m_frames.insert({frame.key(), sheetItem});
    }
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
    }

    if (!gAnimation["layers"].isNull()) {
      for (const auto &gLayer : gAnimation["layers"]) {
        auto layer = parseObjectAnimation(gLayer);
        anim.layers.push_back(layer);
      }
    }
    if (!gAnimation["offsets"].isNull()) {
      for (const auto &gOffset : gAnimation["offsets"]) {
        auto offset = parseIVec2(gOffset.getString());
        anim.offsets.push_back(offset);
      }
    }
    if (!gAnimation["triggers"].isNull()) {
      for (const auto &gTriggers : gAnimation["triggers"]) {
        anim.triggers.push_back(gTriggers.getString());
      }
    }
    return anim;
  }

  static bool toBool(const ngf::GGPackValue &gValue) {
    return !gValue.isNull() && gValue.getInt() == 1;
  }

private:
  enum class State {
    Play,
    Pause
  };

  std::map<std::string, SpriteSheetItem> m_frames;
  std::vector<ObjectAnimation> m_animations;
  ObjectAnimation *m_selectedAnim{nullptr};
  ImGuiTextFilter m_filterAnim;
  ngf::TimeSpan m_elapsed;
  std::shared_ptr<ngf::Texture> m_texture;
  State m_state{State::Pause};
  bool m_left{false};
  Camera m_camera;
  float m_scale{1.f};
};