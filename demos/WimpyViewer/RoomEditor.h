#pragma once
#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <sstream>
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include "Room.h"

class RoomEditor final {
public:
  explicit RoomEditor(Room &room) : m_room(room) {}

  [[nodiscard]] ngf::Color getClearColor() const { return m_clearColor; }

  const Object *getSelectedObject() const { return m_selectedObject; }

  void draw() {
    showMainMenuBar();
    showObjects();
    showObjectProperties();
    showObjectAnimations();
    showAnimationFrames();
    showWalkboxInfo();
  }

private:
  void showMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Quit", "Command|Ctrl+Q")) {
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Edit Properties...", "E", false, m_selectedObject != nullptr)) {
          m_showObjectProperties = true;
        }
        if (ImGui::MenuItem("Edit Animations...", "A", false, m_selectedObject != nullptr)) {
          m_showObjectAnimations = true;
        }
        if (ImGui::MenuItem("Edit Walkboxes...", "W")) {
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View")) {
        if (ImGui::BeginMenu("Show/Hide Layers")) {
          for (const auto &layer : m_room.layers()) {
            auto zsort = layer->getZSort();
            std::ostringstream s;
            s << "Background";
            if (zsort) {
              s << ' ' << zsort;
            }
            auto visible = layer->isVisible();
            if (ImGui::Checkbox(s.str().c_str(), &visible)) {
              layer->setVisible(visible);
            }
          }
          ImGui::EndMenu();
        }
        ImGui::Separator();
        ngf::ImGui::ColorEdit4("Background color", &m_clearColor, ImGuiColorEditFlags_NoInputs);
        ImGui::Separator();
        ImGui::Checkbox("Show Objects...", &m_showObjects);
        ImGui::Checkbox("Show walkbox Info...", &m_showWalkboxInfo);
        ImGui::EndMenu();
      }
      ImGui::EndMainMenuBar();
    }
  }

  void showObjectProperties() {
    if (!m_showObjectProperties)
      return;

    ImGui::Begin("Object Properties", &m_showObjectProperties);
    if (m_selectedObject) {
      ImGui::InputText("object name :", &m_selectedObject->name);
      ImGui::Separator();
      ObjectType(&m_selectedObject->type);
      ImGui::Separator();
      Direction(&m_selectedObject->useDir);
      ImGui::Separator();
      ImGui::DragInt2("Position", glm::value_ptr(m_selectedObject->pos));
      ImGui::DragInt2("Use Position", glm::value_ptr(m_selectedObject->usePos));
      ImGui::DragInt4("Hotspot", &m_selectedObject->hotspot.min.x);
      ImGui::DragInt("ZSort", &m_selectedObject->zsort);
    }
    ImGui::End();
  }

  void showObjectAnimations() {
    if (!m_showObjectAnimations)
      return;

    ImGui::Begin("Animation States", &m_showObjectAnimations);
    if (m_selectedObject) {

      ImGui::Text("Object name : %s", m_selectedObject->name.c_str());
      ImGui::Separator();

      HelpMarker("Drag to rearrange frames.\nRight-click to delete.");
      ImGui::Separator();

      for (auto &anim : m_selectedObject->animations) {
        if (ImGui::TreeNode(anim.name.c_str())) {
          if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("DND_ANIM_FRAME")) {
              IM_ASSERT(payload->DataSize == sizeof(SpriteSheetItem));
              const auto &payload_n = *(const SpriteSheetItem *) payload->Data;
              anim.frames.insert(anim.frames.begin(), payload_n);
            }
            ImGui::EndDragDropTarget();
          }
          for (int i = 0; i < anim.frames.size(); i++) {
            const auto &frame = anim.frames[i];
            ImGui::Button(frame.name.c_str());
            if (ImGui::BeginPopupContextItem()) {
              ImGui::PushID(i);
              ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(0, 0.6f, 0.6f));
              ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor::HSV(0.0f, 0.7f, 0.7f));
              ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor::HSV(0.0f, 0.8f, 0.8f));
              if (ImGui::Button("Delete Frame!")) {
                anim.frames.erase(anim.frames.begin() + i);
              }
              ImGui::PopStyleColor(3);
              ImGui::PopID();
              ImGui::EndPopup();
            }
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
                auto it = std::find_if(anim.frames.cbegin(), anim.frames.cend(), [&payload_n](const auto &item) {
                  return item.name == payload_n.name;
                });
                if (it != anim.frames.cend()) {
                  auto tmp = anim.frames[i];
                  auto n = it - anim.frames.begin();
                  anim.frames[i] = anim.frames[n];
                  anim.frames[n] = tmp;
                } else {
                  anim.frames.insert(anim.frames.begin() + i, payload_n);
                }
              }
              ImGui::EndDragDropTarget();
            }
          }
          ImGui::Checkbox("loop", &anim.loop);
          ImGui::InputDouble("fps", &anim.fps, 1.0f, 1.0f, "%.6f");
          ImGui::TreePop();
        }
      }

      if (ImGui::Button("New...")) {
        ImGui::OpenPopup("New_Anim");
      }

      if (ImGui::BeginPopup("New_Anim")) {
        ImGui::TextUnformatted("New anim name (enter to accept):");
        if (ImGui::InputText("", &m_newAnimName, ImGuiInputTextFlags_EnterReturnsTrue)) {
          if (!m_newAnimName.empty()) {
            m_selectedObject->animations.push_back(ObjectAnimation{m_newAnimName});
            m_newAnimName.clear();
          }
          ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
      }
    }
    ImGui::End();
  }

  void showAnimationFrames() {
    if (!m_showObjectAnimations)
      return;

    ImGui::Begin("Animation Frames", &m_showObjectAnimations);
    ImGui::Separator();
    HelpMarker("Drag to Animation States window to add frames.");
    ImGui::Separator();
    for (const auto &frame : m_room.frames()) {
      ImGui::Button(frame.first.c_str());

      if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("DND_ANIM_FRAME", &frame.second, sizeof(SpriteSheetItem));
        ImGui::Text("%s", frame.first.c_str());
        ImGui::EndDragDropSource();
      }
    }

    ImGui::End();
  }

  void showObjects() {
    if (!m_showObjects)
      return;

    ImGui::Begin("Objects", &m_showObjects);

    m_filterObject.Draw();

    // count the number of objects not filtered
    auto objects = m_room.objects();
    int countObjects = 0;
    for (auto &object : objects) {
      if (!m_filterObject.PassFilter(object.name.c_str()))
        continue;
      countObjects++;
    }

    // display them
    ImGui::Columns(3, nullptr, true);
    auto maxByColumn = std::max(countObjects / 3, 1);
    int count = 0;
    for (auto &object : objects) {
      if (!m_filterObject.PassFilter(object.name.c_str()))
        continue;

      if (count++ >= maxByColumn) {
        ImGui::NextColumn();
        count = 1;
      }

      ImGui::PushID(&object);
      bool isSelected = &object == m_selectedObject;
      ImGui::Checkbox("", &object.visible);
      ImGui::SameLine();
      if (ImGui::Selectable(object.name.c_str(), isSelected)) {
        m_selectedObject = &object;
      }
      if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Button("Edit Properties...")) {
          m_showObjectProperties = true;
        }
        if (ImGui::Button("Edit Animations...")) {
          m_showObjectAnimations = true;
        }
        if (ImGui::Button("Delete Object...")) {
          ImGui::OpenPopup("Delete?");
        }
        if (ImGui::BeginPopupModal("Delete?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
          ImGui::Text("Are you sure you want to delete %s ?", m_selectedObject->name.c_str());
          ImGui::NewLine();
          ImGui::PushID(0);
          ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(0.28f, 0.6f, 0.6f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor::HSV(0.28f, 0.7f, 0.7f));
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor::HSV(.28f, 0.8f, 0.8f));
          if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
          }
          ImGui::SameLine();
          ImGui::PopStyleColor(3);
          ImGui::PopID();
          ImGui::PushID(1);
          ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(0, 0.6f, 0.6f));
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor::HSV(0.0f, 0.7f, 0.7f));
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor::HSV(0.0f, 0.8f, 0.8f));
          if (ImGui::Button("DELETE")) {
            ImGui::CloseCurrentPopup();
          }
          ImGui::PopStyleColor(3);
          ImGui::PopID();
          ImGui::EndPopup();
        }
        ImGui::EndPopup();
      }
      ImGui::PopID();
    }
    ImGui::Columns(1);

    ImGui::End();
  }

  void showWalkboxInfo() {
    if (!m_showWalkboxInfo)
      return;

    ImGui::Begin("Walkbox Info", &m_showWalkboxInfo);

    for (auto &walkbox : m_room.walkboxes()) {
      ImGui::PushID(&walkbox);
      bool isSelected = &walkbox == m_selectedWalkbox;
      auto visible = walkbox.isVisible();
      if (ImGui::Checkbox("", &visible)) {
        walkbox.setVisible(visible);
      }
      ImGui::SameLine();
      auto name = walkbox.getName();
      std::string s = name;
      if (name.empty()) {
        s = "unnamed";
      }
      if (ImGui::Selectable(s.c_str(), isSelected)) {
        m_selectedWalkbox = &walkbox;
      }
      if (ImGui::BeginPopupContextItem()) {
        if (ImGui::Button("Rename...")) {
        }
        if (ImGui::Button("Delete")) {
        }
        ImGui::EndPopup();
      }
      ImGui::PopID();
    }
    ImGui::Columns(1);

    if (ImGui::Button("New walkbox")) {
      std::array<glm::ivec2, 4>
          points{glm::ivec2{200, 100}, glm::ivec2{400, 100}, glm::ivec2{400, 200}, glm::ivec2{200, 200}};
      m_room.walkboxes().push_back(ngf::Walkbox(points));
    }

    ImGui::End();
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

  static void ObjectType(ObjectType *type) {
    bool prop = *type == ObjectType::Prop;
    bool spot = *type == ObjectType::Spot;
    bool trigger = *type == ObjectType::Trigger;
    if (ImGui::Checkbox("Prop", &prop)) {
      *type = prop ? ObjectType::Prop : ObjectType::None;
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Spot", &spot)) {
      *type = spot ? ObjectType::Spot : ObjectType::None;
    }
    ImGui::SameLine();
    if (ImGui::Checkbox("Trigger", &trigger)) {
      *type = trigger ? ObjectType::Trigger : ObjectType::None;
    }
  }

  static void Direction(Direction *direction) {
    if (ImGui::RadioButton("Front", *direction == Direction::Front)) {
      *direction = Direction::Front;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Back", *direction == Direction::Back)) {
      *direction = Direction::Back;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Left", *direction == Direction::Left)) {
      *direction = Direction::Left;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Right", *direction == Direction::Right)) {
      *direction = Direction::Right;
    }
  }

private:
  Room &m_room;
  ngf::Color m_clearColor{ngf::Colors::LightBlue};
  Object *m_selectedObject{nullptr};
  ngf::Walkbox *m_selectedWalkbox{nullptr};
  ImGuiTextFilter m_filterObject;
  bool m_showObjects{false};
  bool m_showObjectProperties{false};
  bool m_showObjectAnimations{false};
  bool m_showWalkboxInfo{false};
  bool m_showNewAnimPopup{false};
  std::string m_newAnimName;

};