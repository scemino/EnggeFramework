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

class WimpyViewerApplication final : public ngf::Application {
public:
  explicit WimpyViewerApplication(std::filesystem::path path)
      : m_path(std::move(path)), m_roomEditor(m_room) {
  }

private:
  void onInit() override {
    m_window.init({"WimpyViewer", {1024, 768}});
    m_room.loadRoom(m_path);
  }

  void onEvent(ngf::Event &event) override {
    if (event.type == ngf::EventType::MouseMoved) {
      m_mousePos = event.mouseMoved.position;
      m_worldPos = getRenderTarget()->mapPixelToCoords(m_mousePos);
    }
  }

  void onRender(ngf::RenderTarget &target) override {
    ngf::View view{ngf::frect::fromCenterSize(m_room.getCamera().size / 2.f,
                                              m_room.getCamera().size)};
    view.zoom(m_zoom);
    target.setView(view);
    target.clear(m_roomEditor.getClearColor());

    ngf::RenderStates states;
    m_room.draw(target, states);

    auto selectedObject = m_roomEditor.getSelectedObject();
    if (selectedObject) {
      drawPosition(target, *selectedObject);
      drawZSort(target, *selectedObject);
      drawHotspot(target, *selectedObject);
    }

    Application::onRender(target);
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

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Mouse pos (%d,%d)", m_mousePos.x, m_mousePos.y);
    ImGui::Text("World pos (%.f,%.f)", m_worldPos.x, m_worldPos.y);

    ImGui::DragFloat2("Position", glm::value_ptr(m_room.getCamera().position));
    ImGui::DragFloat("Zoom", &m_zoom, 0.1f, 0.1f, 10.f);

    ImGui::End();

    m_roomEditor.draw();
  }

private:
  std::filesystem::path m_path;
  Room m_room;
  float m_zoom{1.f};
  glm::ivec2 m_mousePos{};
  glm::vec2 m_worldPos{};
  RoomEditor m_roomEditor;
};

int main(int argc, char **argv) {
  if (argc >= 2) {
    WimpyViewerApplication app{argv[1]};
    app.run();
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}