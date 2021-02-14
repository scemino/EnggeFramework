#include "AnimEditor.h"
#include <imgui.h>
#include <imgui_stdlib.h>
#include <ngf/IO/GGPackValue.h>
#include <ngf/IO/Json/JsonParser.h>
#include <ngf/Application.h>

namespace {
std::string toString(const glm::ivec2 &offset) {
  std::string sOffset;
  sOffset.append("{")
      .append(std::to_string(offset.x)).append(",")
      .append(std::to_string(offset.y)).append("}");
  return sOffset;
}

ngf::GGPackValue toGGPackValue(const ObjectAnimation &anim) {
  ngf::GGPackValue gAnim;
  // add frames
  if (!anim.frames.empty()) {
    ngf::GGPackValue gFrames;
    for (const auto &frame : anim.frames) {
      if (frame.name.empty()) {
        gFrames.push_back("null");
      } else {
        gFrames.push_back(frame.name);
      }
    }
    gAnim["frames"] = gFrames;
  }
  // add layers
  if (!anim.layers.empty()) {
    ngf::GGPackValue gLayers;
    for (const auto &layer : anim.layers) {
      gLayers.push_back(toGGPackValue(layer));
    }
    gAnim["layers"] = gLayers;
  }
  // add triggers
  if (!anim.triggers.empty()) {
    ngf::GGPackValue gTriggers;
    for (const auto &trigger : anim.triggers) {
      if (trigger.empty()) {
        gTriggers.push_back(nullptr);
      } else {
        gTriggers.push_back(trigger);
      }
    }
    gAnim["triggers"] = gTriggers;
  }
  // add offsets
  if (!anim.offsets.empty()) {
    ngf::GGPackValue gOffsets;
    for (const auto &offset : anim.offsets) {
      gOffsets.push_back(toString(offset));
    }
    gAnim["offsets"] = gOffsets;
  }
  if (anim.flags) {
    gAnim["flags"] = anim.flags;
  }
  if (anim.fps) {
    gAnim["fps"] = anim.fps;
  }
  gAnim["name"] = anim.name;
  return gAnim;
}
}

AnimEditor::AnimEditor(ngf::Application &application)
    : m_application(application) {}

void AnimEditor::loadAnim(const std::filesystem::path &path) {
  m_path = path;
  m_selectedAnim = nullptr;
  auto gAnims = ngf::Json::load(path);
  m_sheet = gAnims["sheet"].getString();
  auto sheet = m_sheet + ".png";
  auto sheetPath = path;
  sheetPath = sheetPath.parent_path().append(sheet);

  m_texture = std::make_shared<ngf::Texture>(sheetPath);
  sheetPath = sheetPath.replace_extension(".json");
  loadSheet(sheetPath);

  m_animations = parseObjectAnimations(gAnims["animations"]);
}

void AnimEditor::save() {
  if (m_path.empty())
    return;

  ngf::GGPackValue gAnimations;
  for (const auto &anim : m_animations) {
    gAnimations.push_back(toGGPackValue(anim));
  }

  ngf::GGPackValue gMain;
  gMain["animations"] = gAnimations;
  gMain["sheet"] = m_sheet;

  std::ofstream os(m_path, std::ios::out);
  os << gMain;
  os.close();
}

void AnimEditor::onRender(ngf::RenderTarget &target) {
  ngf::View view{ngf::frect::fromCenterSize(glm::ivec2(160, 90), {320, 180})};
  view.zoom(m_camera.zoom);
  target.setView(view);

  ngf::RenderStates s;
  ngf::Transform t;
  t.setPosition({-m_camera.position.x, m_camera.position.y});
  s.transform *= t.getTransform();

  target.clear(m_clearColor);
  drawAnim(target, s);
}

void AnimEditor::update(const ngf::TimeSpan &e) {
  m_animControl.update(e);
}

void AnimEditor::draw() {
  showMainMenuBar();
  showTools();
  showAnimationFrames();
  showAnimationList();
  showAnimation();
}

void AnimEditor::showMainMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Save", "Command|Ctrl+S")) {
        save();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Quit", "Command|Ctrl+Q")) {
        m_application.quit();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("New Animation...", "N")) {
        newAnimation();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
      ngf::ImGui::ColorEdit4("Background color", &m_clearColor, ImGuiColorEditFlags_NoInputs);
      ImGui::Separator();
      ImGui::Checkbox("Show Tools...", &m_showTools);
      ImGui::Checkbox("Show Animation list...", &m_showAnimations);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void AnimEditor::newAnimation() {
  m_animations.push_back(ObjectAnimation{"newAnimation"});
}

void AnimEditor::update(const ngf::TimeSpan &e, ObjectAnimation &animation) {
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

void AnimEditor::drawAnim(ngf::RenderTarget &target, ngf::RenderStates states) {
  ngf::Transform t;
  t.setPosition({160, 90});
  t.setScale({m_scale, m_scale});
  states.transform *= t.getTransform();

  AnimDrawable animDrawable;
  animDrawable.setAnim(m_selectedAnim);
  animDrawable.setTexture(m_texture);
  animDrawable.setFlipX(m_left);
  animDrawable.draw(target, states);
}

void AnimEditor::draw(const ObjectAnimation &anim, ngf::RenderTarget &target, ngf::RenderStates states) {
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

void AnimEditor::showAnimationList() {
  if (!m_showAnimations)
    return;

  ImGui::Begin("Animation List", &m_showAnimations);

  m_filterAnim.Draw();
  ImGui::Separator();

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

void AnimEditor::showAnimationFrames() {
  if (!m_showAnimations)
    return;

  ImGui::Begin("Animation Frames");
  ImGui::Separator();
  HelpMarker("Drag to Animation States window to add frames.");
  ImGui::Separator();
  m_filterFrame.Draw();
  ImGui::Separator();

  for (const auto &frame : m_frames) {
    if (!m_filterFrame.PassFilter(frame.first.c_str()))
      continue;

    ImGui::Button(frame.first.c_str());

    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
      ImGui::SetDragDropPayload("DND_ANIM_FRAME", &frame.second, sizeof(SpriteSheetItem));
      ImGui::Text("%s", frame.first.c_str());
      ImGui::EndDragDropSource();
    }
  }

  ImGui::End();
}

void AnimEditor::showTools() {
  if (!m_showTools)
    return;

  ImGui::Begin("Tools", &m_showTools);
  ImGui::DragFloat2("Camera Position", &m_camera.position.x);
  ImGui::DragFloat("Camera Zoom", &m_camera.zoom);
  ImGui::DragFloat("Scale", &m_scale);
  ImGui::End();
}

void AnimEditor::showAnimation() {
  if (!m_showAnimations)
    return;

  ImGui::Begin("Animation States");
  if (m_selectedAnim) {
    ImGui::InputText("Animation name", &m_selectedAnim->name);
    ImGui::Separator();

    std::string state;
    switch (m_animControl.getState()) {
    case AnimState::Play:state = "Play";
      break;
    case AnimState::Pause:state = "Pause";
      break;
    case AnimState::Stopped:state = "Stopped";
      break;
    }
    ImGui::Text("State: %s", state.c_str());
    ImGui::Checkbox("Flip vertically", &m_left);
    ImGui::Checkbox("Loop", &m_loop);
    if (ImGui::Button("play")) {
      m_animControl.play(m_loop);
    }
    ImGui::SameLine();
    if (ImGui::Button("pause")) {
      m_animControl.pause();
    }
    ImGui::SameLine();
    if (ImGui::Button("stop")) {
      m_animControl.stop();
    }
    ImGui::Separator();

    HelpMarker("Drag to rearrange frames.\nRight-click to delete.");
    ImGui::Separator();

    if (ImGui::TreeNode("frames")) {

      // you can drop animation frame on it
      if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_ANIM_FRAME")) {
          IM_ASSERT(payload->DataSize == sizeof(SpriteSheetItem));
          const auto &payload_n = *(const SpriteSheetItem *) payload->Data;
          m_selectedAnim->frames.insert(m_selectedAnim->frames.begin(), payload_n);
        }
        ImGui::EndDragDropTarget();
      }

      ImGui::Checkbox("loop", &m_selectedAnim->loop);
      ImGui::InputInt("fps", &m_selectedAnim->fps, 1, 2);
      ImGui::InputInt("flags", &m_selectedAnim->flags, 1, 2);

      showFrames(m_selectedAnim->frames);
      ImGui::TreePop();
    }
    if (ImGui::TreeNode("layers")) {
      if (ImGui::Button("New...")) {
        ImGui::OpenPopup("New_Layer");
      }

      if (ImGui::BeginPopup("New_Layer")) {
        ImGui::TextUnformatted("New layer name (enter to accept):");
        if (ImGui::InputText("", &m_newLayerName, ImGuiInputTextFlags_EnterReturnsTrue)) {
          if (!m_newLayerName.empty()) {
            newLayer(m_newLayerName);
          }
          ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
      }

      // draw layer
      for (int i = 0; i < m_selectedAnim->layers.size(); i++) {
        auto &layer = m_selectedAnim->layers[i];
        ImGui::PushID(&layer);
        ImGui::Checkbox("", &layer.visible);
        ImGui::SameLine();
        auto expanded = ImGui::TreeNode(layer.name.c_str());
        ImGui::PopID();

        // you can drop animation frame on it
        if (ImGui::BeginDragDropTarget()) {
          if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_ANIM_FRAME")) {
            IM_ASSERT(payload->DataSize == sizeof(SpriteSheetItem));
            const auto &payload_n = *(const SpriteSheetItem *) payload->Data;
            layer.frames.insert(layer.frames.begin(), payload_n);
          }
          ImGui::EndDragDropTarget();
        }

        // layer context menu
        if (ImGui::BeginPopupContextItem()) {
          ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(0, 0.6f, 0.6f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor::HSV(0.0f, 0.7f, 0.7f));
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor::HSV(0.0f, 0.8f, 0.8f));
          if (ImGui::Button("Delete")) {
            m_selectedAnim->layers.erase(m_selectedAnim->layers.begin() + i);
          }
          ImGui::PopStyleColor(3);
          ImGui::EndPopup();
        }

        // layer properties
        if (expanded) {
          ImGui::Checkbox("loop", &layer.loop);
          ImGui::InputInt("fps", &layer.fps, 1, 2);
          ImGui::InputInt("flags", &layer.flags, 1, 2);
          showFrames(layer.frames);
          ImGui::TreePop();
        }
      }
      ImGui::TreePop();
    }
  }
  ImGui::End();
}

void AnimEditor::newLayer(const std::string &name) {
  m_selectedAnim->layers.push_back(ObjectAnimation{name});
}

void AnimEditor::showFrames(std::vector<SpriteSheetItem> &items) {
  for (int i = 0; i < items.size(); i++) {
    const auto &frame = items[i];
    ImGui::Button(frame.name.c_str());

    // frame context menu
    if (ImGui::BeginPopupContextItem()) {
      ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(0, 0.6f, 0.6f));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor::HSV(0.0f, 0.7f, 0.7f));
      ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor::HSV(0.0f, 0.8f, 0.8f));
      if (ImGui::Button("Delete")) {
        items.erase(items.begin() + i);
      }
      ImGui::PopStyleColor(3);
      ImGui::EndPopup();
    }

    // frame drag source
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
      ImGui::SetDragDropPayload("DND_ANIM_FRAME",
                                &frame,
                                sizeof(SpriteSheetItem));    // Set payload to carry the index of our item (could be anything)
      ImGui::Text("%s", frame.name.c_str());
      ImGui::EndDragDropSource();
    }

    // frame drop target
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

void AnimEditor::setSelectAnim(ObjectAnimation *pAnim) {
  m_selectedAnim = pAnim;
  m_animControl.setAnimation(pAnim);
}

void AnimEditor::HelpMarker(const char *desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}

void AnimEditor::loadSheet(const std::filesystem::path &path) {
  m_frames.clear();
  auto sheetJson = ngf::Json::load(path);
  const auto &frames = sheetJson["frames"];
  for (const auto &frame : frames.items()) {
    auto sheetItem = toSpriteSheetItem(frame.key(), frame.value());
    m_frames.insert({frame.key(), sheetItem});
  }
}

SpriteSheetItem AnimEditor::toSpriteSheetItem(const std::string &name, const ngf::GGPackValue &value) {
  auto rect = toRect(value["frame"]);
  auto spriteSourceSize = toRect(value["spriteSourceSize"]);
  auto sourceSize = toSize(value["sourceSize"]);
  return SpriteSheetItem{name, rect, spriteSourceSize, sourceSize};
}

ngf::irect AnimEditor::toRect(const ngf::GGPackValue &value) {
  auto x = value["x"].getInt();
  auto y = value["y"].getInt();
  auto w = value["w"].getInt();
  auto h = value["h"].getInt();
  return ngf::irect::fromPositionSize({x, y}, {w, h});
}

glm::ivec2 AnimEditor::toSize(const ngf::GGPackValue &value) {
  auto w = value["w"].getInt();
  auto h = value["h"].getInt();
  return glm::ivec2({w, h});
}

glm::ivec2 AnimEditor::parseIVec2(std::string_view value) {
  char *ptr;
  auto x = std::strtol(value.data() + 1, &ptr, 10);
  auto y = std::strtol(ptr + 1, &ptr, 10);
  return glm::ivec2{x, y};
}

std::vector<ObjectAnimation> AnimEditor::parseObjectAnimations(const ngf::GGPackValue &gAnimations) {
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

ObjectAnimation AnimEditor::parseObjectAnimation(const ngf::GGPackValue &gAnimation) {
  ObjectAnimation anim;
  anim.name = gAnimation["name"].getString();
  anim.loop = toBool(gAnimation["loop"]);
  anim.fps = gAnimation["fps"].isNull() ? 0.f : gAnimation["fps"].getInt();
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

bool AnimEditor::toBool(const ngf::GGPackValue &gValue) {
  return !gValue.isNull() && gValue.getInt() == 1;
}

enum class State {
  Play,
  Pause
};

