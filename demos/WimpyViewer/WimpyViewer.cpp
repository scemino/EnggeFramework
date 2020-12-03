#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/CircleShape.h>
#include <imgui.h>
#include <utility>
#include "SpriteSheetItem.h"
#include "Camera.h"
#include "Room.h"
#include "RoomEditor.h"
#include "CameraControl.h"

class WimpyViewerApplication final : public ngf::Application {
public:
  explicit WimpyViewerApplication(std::filesystem::path path)
      : m_path(std::move(path)), m_roomEditor(*this, m_room), m_cameraControl(m_room) {
    m_roomEditor.onSelectedObjectChanged([](auto pObj) {
      if (pObj)
        pObj->pause();
    });
  }

private:
  void onInit() override {
    m_window.init({"WimpyViewer", {1024, 768}});
    auto &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    m_room.loadRoom(m_path);
  }

  void onEvent(ngf::Event &event) override {
    switch (event.type) {
    case ngf::EventType::MouseMoved: {
      m_mousePos = event.mouseMoved.position;
      const auto scale = ngf::Window::getDpiScale();
      m_worldPos = getRenderTarget()->mapPixelToCoords(m_mousePos) * scale;
      m_worldPos.y = m_room.getSize().y - m_worldPos.y;
      m_worldPos += m_room.getCamera().position;
      if (m_walkboxVertex.walkbox) {
        auto it = m_walkboxVertex.walkbox->begin() + m_walkboxVertex.vertexIndex;
        (*it) = m_worldPos;
      }
    }
      break;
    case ngf::EventType::MouseButtonPressed: {
      if (m_cameraControl.isPanOrZoomEnabled())
        break;
      auto pWalkbox = getWalkboxAt(m_worldPos);
      if (pWalkbox) {
        m_roomEditor.setSelectedWalkbox(pWalkbox);
      }
      m_walkboxVertex = getWalkboxVertexAt(m_worldPos);
    }
      break;
    case ngf::EventType::MouseButtonReleased: {
      m_walkboxVertex.walkbox = nullptr;
    }
      break;
    case ngf::EventType::KeyPressed: {
      auto pObj = m_roomEditor.getSelectedObject();
      if (pObj && !pObj->animations.empty() && event.key.scancode >= ngf::Scancode::D1
          && event.key.scancode <= ngf::Scancode::D0) {
        auto digit = static_cast<int>(event.key.scancode) - static_cast<int>(ngf::Scancode::D1);
        if (digit < pObj->animations.size()) {
          pObj->animationIndex = digit;
          pObj->play();
        }
      }
    }
      break;
    default:break;
    }
    m_cameraControl.onEvent(*this, event);
  }

  void onUpdate(const ngf::TimeSpan &elapsed) override {
    m_room.update(elapsed);
  }

  void onRender(ngf::RenderTarget &target) override {
    target.clear(m_roomEditor.getClearColor());

    // draw room
    ngf::RenderStates states;
    m_room.draw(target, states);

    // draw walkboxes
    const auto screenSize = m_room.getScreenSize();
    const auto offsetY = screenSize.y - m_room.getSize().y;
    const auto &camera = m_room.getCamera();
    ngf::RenderStates wbStates = states;
    ngf::Transform t;
    t.setPosition({-camera.position.x, camera.position.y + offsetY});
    wbStates.transform *= t.getTransform();
    for (const auto &walkbox: m_room.walkboxes()) {
      if (!walkbox.isVisible())
        continue;

      const auto color = m_roomEditor.getSelectedWalkbox() == &walkbox ? ngf::Colors::LimeGreen : ngf::Colors::Green;
      std::vector<ngf::Vertex> polygon;
      std::transform(walkbox.cbegin(), walkbox.cend(), std::back_inserter(polygon), [this, color](const auto &p) {
        return ngf::Vertex{{p.x, m_room.getSize().y - p.y}, color};
      });
      target.draw(ngf::PrimitiveType::LineLoop, polygon, wbStates);
    }

    // draw position, zsort line and hotspot if an object is selected
    auto selectedObject = m_roomEditor.getSelectedObject();
    if (selectedObject) {
      drawPosition(target, *selectedObject);
      drawZSort(target, *selectedObject);
      drawHotspot(target, *selectedObject);
    }

    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Mouse pos (%d,%d)", m_mousePos.x, m_mousePos.y);
    ImGui::Text("World pos (%.f,%.f)", m_worldPos.x, m_worldPos.y);

    ImGui::DragFloat2("Position", glm::value_ptr(m_room.getCamera().position));
    ImGui::DragFloat("Zoom", &m_room.getCamera().zoom, 0.1f, 0.1f, 10.f);

    ImGui::End();

    m_roomEditor.draw();
  }

  ngf::Walkbox *getWalkboxAt(glm::vec2 pos) {
    for (auto &wb : m_room.walkboxes()) {
      if (wb.inside(m_worldPos)) {
        return &wb;
      }
    }
    return nullptr;
  }

  struct WalkboxVertexHitTest {
  public:
    ngf::Walkbox *walkbox{nullptr};
    int vertexIndex{-1};
  };

  WalkboxVertexHitTest getWalkboxVertexAt(glm::vec2 pos) {
    for (auto& wb : m_room.walkboxes()) {
      for (auto it = wb.cbegin(); it != wb.cend(); it++) {
        auto rect = ngf::frect::fromCenterSize(*it, glm::vec2{5, 5});
        if (rect.contains(m_worldPos)) {
          const auto index = static_cast<int>(std::distance(wb.cbegin(), it));
          return {&wb, index};
        }
      }
    }
    return {};
  }

  [[nodiscard]] static ngf::Color getColor(ObjectType type) {
    switch (type) {
    case ObjectType::Prop:return ngf::Colors::Blue;
    case ObjectType::Spot:return ngf::Colors::Green;
    case ObjectType::Trigger:return ngf::Colors::Orange;
    default:return ngf::Colors::Red;
    }
  }

  static void drawPosition(ngf::RenderTarget &target, const Object &object) {
    if (object.type == ObjectType::Trigger)
      return;

    auto color = getColor(object.type);

    constexpr float length = 8.f;
    constexpr float length_2 = length / 2.f;
    constexpr float length_4 = length / 4.f;
    std::array<ngf::Vertex, 4> posVertices = {
        ngf::Vertex{{-length_2, 0.f}, color},
        ngf::Vertex{{length_2, 0.f}, color},
        ngf::Vertex{{0.f, -length_2}, color},
        ngf::Vertex{{0.f, length_2}, color}};

    ngf::RenderStates states;
    ngf::Transform tObj;
    tObj.setPosition({object.pos.x + object.usePos.x, object.pos.y - object.usePos.y});
    states.transform = tObj.getTransform();
    target.draw(ngf::PrimitiveType::Lines, posVertices, states);

    if (object.useDir != Direction::None) {
      std::array<ngf::Vertex, 2> dirVertices;
      switch (object.useDir) {
      case Direction::Back:dirVertices[0] = {{-length_4, -length_4}};
        dirVertices[1] = {{length_4, -length_4}};
        break;
      case Direction::Front:dirVertices[0] = {{-length_4, length_4}};
        dirVertices[1] = {{length_4, length_4}};
        break;
      case Direction::Left:dirVertices[0] = {{-length_4, -length_4}};
        dirVertices[1] = {{-length_4, length_4}};
        break;
      case Direction::Right:dirVertices[0] = {{length_4, -length_4}};
        dirVertices[1] = {{length_4, length_4}};
        break;
      default:break;
      }
      dirVertices[0].color = color;
      dirVertices[1].color = color;
      target.draw(ngf::PrimitiveType::Lines, dirVertices, states);

      ngf::CircleShape circle(1.0f);
      circle.setAnchor(ngf::Anchor::Center);
      circle.setColor(color);
      circle.draw(target, states);
    }
  }

  static void drawHotspot(ngf::RenderTarget &target, const Object &object) {
    if (object.type != ObjectType::None && object.type != ObjectType::Trigger)
      return;

    ngf::Transform tObj;
    tObj.setPosition(object.pos);

    auto color = getColor(object.type);
    std::array<ngf::Vertex, 4> hotspotVertices = {
        ngf::Vertex{object.hotspot.getTopLeft(), color},
        ngf::Vertex{object.hotspot.getBottomLeft(), color},
        ngf::Vertex{object.hotspot.getBottomRight(), color},
        ngf::Vertex{object.hotspot.getTopRight(), color}};

    // draw hotspot and position
    ngf::RenderStates states;
    states.transform = tObj.getTransform();
    target.draw(ngf::PrimitiveType::LineLoop, hotspotVertices, states);

    // draw circle on each vertex
    ngf::CircleShape circle(1.0f);
    circle.setColor(color);
    circle.setAnchor(ngf::Anchor::Center);
    for (auto &vertex : hotspotVertices) {
      circle.getTransform().setPosition(vertex.pos);
      circle.draw(target, states);
    }
  }

  static void drawZSort(ngf::RenderTarget &target, const Object &object) {
    const auto color = getColor(object.type);
    const auto y = object.zsort;
    std::array<ngf::Vertex, 2> vertices;
    vertices[0] = {{0, y}, color};
    vertices[1] = {{target.getSize().x, y}, color};
    target.draw(ngf::PrimitiveType::Lines, vertices);

    ngf::CircleShape circle(1.0f);
    circle.getTransform().setPosition({5.f, y});
    circle.setColor(color);
    circle.setAnchor(ngf::Anchor::Center);
    circle.draw(target, {});

    circle.getTransform().setPosition({target.getSize().x - 5.f, y});
    circle.draw(target, {});
  }

private:
  std::filesystem::path m_path;
  Room m_room;
  glm::ivec2 m_mousePos{};
  glm::vec2 m_worldPos{};
  RoomEditor m_roomEditor;
  CameraControl m_cameraControl;
  WalkboxVertexHitTest m_walkboxVertex;
};

int main(int argc, char **argv) {
  if (argc >= 2) {
    WimpyViewerApplication app{argv[1]};
    app.run();
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}