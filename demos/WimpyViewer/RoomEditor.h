#pragma once
#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <ngf/Application.h>
#include <functional>
#include <sstream>
#include <utility>
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include "CameraControl.h"
#include "Room.h"

struct WalkboxVertexHitTest {
public:
  ngf::Walkbox *walkbox{nullptr};
  int vertexIndex{-1};
};

enum class ObjectHit {
  None,
  Position,
  UsePosition,
  Hotspot,
  HotspotVertex,
  Zsort
};

struct ObjectHitTest {
public:
  Object *pObj{nullptr};
  ObjectHit hit{ObjectHit::None};
  glm::vec2 startPos{};
  ngf::irect startHotspot{};
  int vertexIndex{0};
  int startZsort{0};
};

class RoomEditor final {
public:
  explicit RoomEditor(ngf::Application &application, Room &room)
      : m_application(application), m_room(room), m_cameraControl(m_room) {}

  [[nodiscard]] ngf::Color getClearColor() const { return m_clearColor; }

  [[nodiscard]] Object *getSelectedObject() { return m_selectedObject; }

  void openWimpy(const std::string &path) {
    try {
      m_path = path;
      m_room.loadRoom(path);
    } catch (const std::exception &exception) {
      std::ostringstream s;
      s << "Invalid wimpy file (" << path << ") : " << exception.what();
      ngf::Application::showMessageBox("Error", s.str(), ngf::MessageBoxType::Warning, &m_application.getWindow());
    }
  }

  void onSelectedObjectChanged(std::function<void(Object *)> callback) {
    m_selectedObjectChanged = std::move(callback);
  }

  void setSelectedWalkbox(ngf::Walkbox *walkbox) { m_selectedWalkbox = walkbox; }
  [[nodiscard]] ngf::Walkbox *getSelectedWalkbox() { return m_selectedWalkbox; }

  [[nodiscard]] bool isWalkboxInEdition() const noexcept { return m_walkboxInEdition; }
  [[nodiscard]] bool isModified() const noexcept { return m_isModified; }

  void draw() {
    showMainMenuBar();
    showObjects();
    showObjectProperties();
    showObjectAnimations();
    showAnimationFrames();
    showWalkboxInfo();

    if (m_quit && isModified()) {
      bool quitAccepted = false;
      if (!ImGui::IsPopupOpen("Quit!"))
        ImGui::OpenPopup("Quit!");
      if (ImGui::BeginPopupModal("Quit!")) {
        ImGui::Text("Discard changes to '%s'?", m_path.c_str());
        ImGui::NewLine();
        ImGui::PushID(0);
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(0.28f, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor::HSV(0.28f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor::HSV(.28f, 0.8f, 0.8f));
        if (ImGui::Button("Cancel")) {
          m_quit = false;
          ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        ImGui::PopStyleColor(3);
        ImGui::PopID();
        ImGui::PushID(1);
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4) ImColor::HSV(0, 0.6f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4) ImColor::HSV(0.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4) ImColor::HSV(0.0f, 0.8f, 0.8f));
        if (ImGui::Button("Discard!")) {
          quitAccepted = true;
          ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor(3);
        ImGui::PopID();
        ImGui::EndPopup();
      }
      if (quitAccepted) {
        m_application.quit();
      }
    }
  }

  void onEvent(ngf::Event &event) {
    switch (event.type) {
    case ngf::EventType::DropFile: {
      openWimpy(event.drop.file);
    }
      break;
    case ngf::EventType::MouseMoved: {
      if (!m_room.isLoaded())
        break;
      m_mousePos = event.mouseMoved.position;
      const auto scale = ngf::Window::getDpiScale();
      m_worldPos = m_application.getRenderTarget()->mapPixelToCoords(m_mousePos) * scale;
      m_worldPos.x += m_room.getCamera().position.x;
      m_worldPos.y = m_room.getCamera().position.y + m_room.getScreenSize().y - m_worldPos.y;
      if (m_walkboxVertex.walkbox) {
        auto it = m_walkboxVertex.walkbox->begin() + m_walkboxVertex.vertexIndex;
        (*it) = m_worldPos;
        setModified();
      } else if (m_objectHitTest.pObj) {
        setModified();
        switch (m_objectHitTest.hit) {
        case ObjectHit::Position:m_objectHitTest.pObj->pos = m_worldPos;
          break;
        case ObjectHit::UsePosition:
          m_objectHitTest.pObj->usePos =
              {m_worldPos.x - m_objectHitTest.pObj->pos.x,
               m_worldPos.y - m_objectHitTest.pObj->pos.y};
          break;
        case ObjectHit::Hotspot: {
          auto delta = m_worldPos - m_objectHitTest.startPos;
          auto hotspot = m_objectHitTest.startHotspot;
          m_objectHitTest.pObj->hotspot =
              ngf::irect::fromPositionSize({hotspot.getPosition().x + delta.x, hotspot.getPosition().y + delta.y},
                                           hotspot.getSize());
        }
          break;
        case ObjectHit::HotspotVertex: {
          auto delta = m_worldPos - m_objectHitTest.startPos;
          auto hotspot = m_objectHitTest.startHotspot;
          switch (m_objectHitTest.vertexIndex) {
          case 0:hotspot.min.x += delta.x;
            hotspot.max.y += delta.y;
            break;
          case 1:hotspot.max.x += delta.x;
            hotspot.max.y += delta.y;
            break;
          case 2:hotspot.max.x += delta.x;
            hotspot.min.y += delta.y;
            break;
          case 3:hotspot.min.x += delta.x;
            hotspot.min.y += delta.y;
            break;
          }
          m_objectHitTest.pObj->hotspot = hotspot;
        }
          break;
        case ObjectHit::Zsort: {
          auto delta = m_worldPos - m_objectHitTest.startPos;
          auto zsort = m_objectHitTest.startZsort;
          m_objectHitTest.pObj->zsort = zsort + delta.y;
        }
          break;
        default:break;
        }
      }
    }
      break;
    case ngf::EventType::MouseButtonPressed: {
      if (m_cameraControl.isPanOrZoomEnabled())
        break;

      // test if we selected a walkbox
      auto pWalkbox = getWalkboxAt(m_worldPos);
      if (pWalkbox) {
        setSelectedWalkbox(pWalkbox);
      }
      // test if we are near a walkbox vertex
      m_walkboxVertex = getWalkboxVertexAt(m_worldPos);
      if (event.mouseButton.button == 3 && m_walkboxVertex.walkbox) {
        auto it = m_walkboxVertex.walkbox->begin() + m_walkboxVertex.vertexIndex;
        m_walkboxVertex.walkbox->erase(it);
        break;
      }

      // test if mouse right button is pressed near a walkbox
      if (event.mouseButton.button == 3 && !m_walkboxVertex.walkbox) {
        for (auto &wb : m_room.walkboxes()) {
          auto edgePoint = wb.getClosestPointOnEdge(m_worldPos);
          if (glm::distance(edgePoint, m_worldPos) < 5.f) {
            // yes, so add a vertex in this walkbox
            auto index = getIndexWhereToInsertPoint(wb, edgePoint);
            auto it = wb.cbegin() + index;
            wb.insert(it, edgePoint);
            break;
          }
        }
      }

      // test if we are near an object: position, hotspot or zsort
      auto pObj = getSelectedObject();
      if (pObj && !isWalkboxInEdition())
        objectHitTest(pObj);
    }
      break;
    case ngf::EventType::MouseButtonReleased: {
      m_walkboxVertex.walkbox = nullptr;
      m_objectHitTest.pObj = nullptr;
    }
      break;
    case ngf::EventType::KeyPressed: {
      auto pObj = getSelectedObject();
      if (event.key.modifiers == ngf::KeyModifiers::None) {
        if (pObj && !pObj->animations.empty() && event.key.scancode >= ngf::Scancode::D1
            && event.key.scancode <= ngf::Scancode::D0) {
          auto digit = static_cast<int>(event.key.scancode) - static_cast<int>(ngf::Scancode::D1);
          if (digit < pObj->animations.size()) {
            pObj->animationIndex = digit;
            pObj->play();
          }
        } else if (event.key.keycode == ngf::Keycode::N) {
          newObject();
        } else if (event.key.keycode == ngf::Keycode::A) {
          m_showObjectAnimations = true;
        } else if (event.key.keycode == ngf::Keycode::C) {
          centerObject();
        } else if (event.key.keycode == ngf::Keycode::E) {
          m_showObjectProperties = true;
        } else if (event.key.keycode == ngf::Keycode::W) {
          m_walkboxInEdition = !m_walkboxInEdition;
        }
      } else {
        if (event.key.modifiers == ngf::KeyModifiers::LeftGui && event.key.keycode == ngf::Keycode::S) {
          save();
        }
        if (event.key.modifiers == ngf::KeyModifiers::LeftGui && event.key.keycode == ngf::Keycode::Q) {
          onQuit();
        }
        if (event.key.modifiers == ngf::KeyModifiers::LeftShift && event.key.keycode == ngf::Keycode::W) {
          newWalkbox();
        }
      }
    }
      break;
    default:break;
    }
    m_cameraControl.onEvent(m_application, event);
  }

  void onQuit() {
    m_quit = true;
  }

private:
  void setModified(bool isModified = true) {
    ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg] =
        isModified ? ImVec4(ImColor(0x882a12ff)) : ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    m_isModified = isModified;
  }

  void save() {
    m_room.save();
    setModified(false);
  }

  void showMainMenuBar() {
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
        if (ImGui::MenuItem("New Object...", "N")) {
          newObject();
        }
        if (ImGui::MenuItem("New Walkbox...", "Shift-W")) {
          newWalkbox();
        }
        if (ImGui::MenuItem("Edit Properties...", "E", false, m_selectedObject != nullptr)) {
          m_showObjectProperties = true;
        }
        if (ImGui::MenuItem("Edit Animations...", "A", false, m_selectedObject != nullptr)) {
          m_showObjectAnimations = true;
        }
        if (ImGui::MenuItem("Edit Walkboxes...", "W", m_walkboxInEdition)) {
          m_walkboxInEdition = !m_walkboxInEdition;
        }
        ImGui::EndMenu();
      }
      if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Center Object", "C", false, m_selectedObject != nullptr)) {
          centerObject();
        }
        ImGui::Separator();
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
        ImGui::Checkbox("Autocenter Objects...", &m_autocenterObjects);
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
      if (ImGui::DragInt2("Position", glm::value_ptr(m_selectedObject->pos))) {
        setModified();
      }
      if (ImGui::DragInt2("Use Position", glm::value_ptr(m_selectedObject->usePos))) {
        setModified();
      }
      if (ImGui::DragInt4("Hotspot", &m_selectedObject->hotspot.min.x)) {
        setModified();
      }
      if (ImGui::DragInt("ZSort", &m_selectedObject->zsort)) {
        setModified();
      }
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
              setModified();
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
                setModified();
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
                setModified();
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
            setModified();
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
        setSelectObject(&object);
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
            auto it = std::find_if(objects.cbegin(), objects.cend(), [this](const auto &obj) {
              return &obj == m_selectedObject;
            });
            m_selectedObject = nullptr;
            m_room.objects().erase(it);
            setModified();
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
          ImGui::OpenPopup("Rename_Walkbox");
        }

        if (ImGui::BeginPopup("Rename_Walkbox")) {
          m_newWalkboxName = m_selectedWalkbox->getName();
          if (m_newWalkboxName.empty()) {
            m_newWalkboxName = "unnamed";
          }
          ImGui::TextUnformatted("Name (enter to accept):");
          ImGui::SetKeyboardFocusHere(0);
          if (ImGui::InputText("",
                               &m_newWalkboxName,
                               ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
            if (!m_newWalkboxName.empty()) {
              m_selectedWalkbox->setName(m_newWalkboxName);
            }
            ImGui::CloseCurrentPopup();
          }
          ImGui::EndPopup();
        }

        if (ImGui::Button("Delete")) {
          auto walkboxes = m_room.walkboxes();
          auto it = std::find_if(walkboxes.cbegin(), walkboxes.cend(), [this](const auto &wb) {
            return &wb == m_selectedWalkbox;
          });
          walkboxes.erase(it);
        }
        ImGui::EndPopup();
      }
      ImGui::PopID();
    }
    ImGui::Columns(1);

    if (ImGui::Button("New walkbox")) {
      newWalkbox();
    }

    ImGui::End();
  }

  void newWalkbox() {
    glm::vec2 center = {m_room.getScreenSize().x / 2.f, m_room.getScreenSize().y / 2.f};
    std::array<glm::ivec2, 4> points{
        glm::ivec2{center.x - 40, center.y - 20},
        glm::ivec2{center.x + 40, center.y - 20},
        glm::ivec2{center.x + 40, center.y + 20},
        glm::ivec2{center.x - 40, center.y + 20}};
    m_room.walkboxes().push_back(ngf::Walkbox(points));
    m_selectedWalkbox = nullptr;
    setModified();
  }

  void newObject() {
    glm::vec2 center = {m_room.getScreenSize().x / 2.f, m_room.getScreenSize().y / 2.f};
    Object obj;
    obj.room = &m_room;
    obj.texture = m_room.getTexture();
    obj.name = "nameObject";
    obj.zsort = static_cast<int>(m_room.getScreenSize().y / 2.f);
    obj.pos = center;
    obj.hotspot = ngf::irect::fromMinMax({-20, 20}, {40, 40});
    m_room.objects().push_back(obj);
    setSelectObject(&m_room.objects().back());
    m_showObjectProperties = true;
    setModified();
  }

  void centerObject() {
    if (!m_selectedObject)
      return;
    auto pos = m_selectedObject->pos;
    glm::vec2 screenSize = m_room.getScreenSize();
    m_room.getCamera().position =
        (glm::vec2) glm::vec2(m_selectedObject->pos.x - screenSize.x / 2.f,
                              screenSize.y / 2.f - m_selectedObject->pos.y);
  }

  void setSelectObject(Object *pObj) {
    auto previousObject = m_selectedObject;
    m_selectedObject = pObj;
    if (m_autocenterObjects) {
      centerObject();
    }
    if (m_selectedObjectChanged) {
      m_selectedObjectChanged(previousObject);
    }
  }

  ngf::Walkbox *getWalkboxAt(glm::vec2 pos) {
    for (auto &wb : m_room.walkboxes()) {
      if (wb.inside(pos)) {
        return &wb;
      }
    }
    return nullptr;
  }

  WalkboxVertexHitTest getWalkboxVertexAt(glm::vec2 pos) {
    for (auto &wb : m_room.walkboxes()) {
      for (auto it = wb.cbegin(); it != wb.cend(); it++) {
        auto rect = ngf::frect::fromCenterSize(*it, glm::vec2{5, 5});
        if (rect.contains(pos)) {
          const auto index = static_cast<int>(std::distance(wb.cbegin(), it));
          return {&wb, index};
        }
      }
    }
    return {};
  }

  void objectHitTest(Object *pObj) {
    assert(pObj);

    m_objectHitTest.hit = ObjectHit::None;
    m_objectHitTest.startPos = m_worldPos;
    m_objectHitTest.startHotspot = pObj->hotspot;
    m_objectHitTest.startZsort = pObj->zsort;
    ObjectHit hit;
    ngf::frect rect;

    // if this a prop, we want to modify the position of the object
    if (pObj->type == ObjectType::Prop) {
      hit = ObjectHit::Position;
      rect = ngf::frect::fromCenterSize(
          {pObj->pos.x, pObj->pos.y},
          {5, 5});
    } else if (pObj->type != ObjectType::Trigger) {
      // else we want to modify the use position of the object (except for trigger)
      hit = ObjectHit::UsePosition;
      rect = ngf::frect::fromCenterSize(
          {pObj->pos.x + pObj->usePos.x, pObj->pos.y - pObj->usePos.y},
          {5, 5});
    }

    if (rect.contains(m_worldPos)) {
      m_objectHitTest.pObj = pObj;
      m_objectHitTest.hit = hit;
      return;
    }

    // test if mouse is over hotspot vertex
    auto hotspot = ngf::frect::fromMinMax(pObj->pos + pObj->hotspot.min, pObj->pos + pObj->hotspot.max);
    std::array<glm::vec2, 4>
        positions = {hotspot.getBottomLeft(), hotspot.getBottomRight(),
                     hotspot.getTopRight(), hotspot.getTopLeft()};
    for (auto i = 0; i < 4; i++) {
      auto vertexPos = positions[i];
      if (glm::distance(vertexPos, m_worldPos) < 5.f) {
        m_objectHitTest.pObj = pObj;
        m_objectHitTest.hit = ObjectHit::HotspotVertex;
        m_objectHitTest.vertexIndex = i;
        return;
      }
    }

    // test if mouse is over object's hotspot
    if (hotspot.contains(m_worldPos)) {
      m_objectHitTest.pObj = pObj;
      m_objectHitTest.hit = ObjectHit::Hotspot;
      return;
    }

    // test if mouse is over object's zsort
    if (fabs(m_worldPos.y - static_cast<float>(pObj->zsort)) < 5.f) {
      m_objectHitTest.pObj = pObj;
      m_objectHitTest.hit = ObjectHit::Zsort;
      return;
    }
  }

  static int getIndexWhereToInsertPoint(const ngf::Walkbox &wb, glm::ivec2 edgePoint) {
    std::vector<float> dists(wb.size());
    for (int i = 0; i < wb.size(); i++) {
      dists[i] = ngf::Walkbox::distanceToSegment(edgePoint, wb.at(i), wb.at((i + 1) % wb.size()));
    }
    auto dIt = std::min_element(dists.cbegin(), dists.cend());
    return static_cast<int>((std::distance(dists.cbegin(), dIt) + 1) % wb.size());
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
  ngf::Application &m_application;
  Room &m_room;
  ngf::Color m_clearColor{ngf::Colors::LightBlue};
  Object *m_selectedObject{nullptr};
  ngf::Walkbox *m_selectedWalkbox{nullptr};
  ImGuiTextFilter m_filterObject;
  bool m_showObjects{false};
  bool m_showObjectProperties{false};
  bool m_showObjectAnimations{false};
  bool m_showWalkboxInfo{false};
  std::string m_newAnimName;
  std::string m_newWalkboxName;
  std::function<void(Object *)> m_selectedObjectChanged{nullptr};
  bool m_walkboxInEdition{false};
  bool m_autocenterObjects{false};
  bool m_isModified{false};
  glm::ivec2 m_mousePos{};
  glm::vec2 m_worldPos{};
  WalkboxVertexHitTest m_walkboxVertex;
  ObjectHitTest m_objectHitTest;
  CameraControl m_cameraControl;
  bool m_quit{false};
  std::string m_path;
};