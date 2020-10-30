#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Vertex.h>
#include <ngf/Math/PathFinding/Walkbox.h>
#include <imgui.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <Math/PathFinding/PathFinder.h>

constexpr glm::ivec2 points[] =
    {{3, 76}, {27, 76}, {28, 69}, {39, 70}, {50, 67}, {64, 77},
     {83, 75}, {91, 81}, {109, 86}, {111, 93}, {128, 94}, {190, 69},
     {318, 69}, {318, 54}, {4, 54}};

constexpr auto pointsSize = sizeof(points) / sizeof(points[0]);

class DemoApplication final : public ngf::Application {
public:
  DemoApplication()
      : m_walkbox(points, points + pointsSize),
        m_walkboxes(&m_walkbox, &m_walkbox + 1),
        m_pathFinder(m_walkboxes) {
    m_vertices.reserve(pointsSize);
    std::transform(points, points + pointsSize, std::back_inserter(m_vertices), [](auto pt) {
      return ngf::Vertex{.pos={pt.x, pt.y}};
    });
  }

private:
  void onInit() override {
    m_window.init({.title="10 - Path finding", .size={1024, 780}});
  }

  void onEvent(ngf::Event &event) override {
    if (event.type == ngf::EventType::MouseMoved) {
      m_mousePos = event.mouseMoved.position;
      m_worldPos = getRenderTarget()->mapPixelToCoords(m_mousePos);
      const auto lastInside = m_isInside;
      m_isInside = m_walkbox.inside(m_worldPos);
      if (lastInside != m_isInside) {
        std::for_each(m_vertices.begin(), m_vertices.end(), [this](auto &vertex) {
          vertex.color = m_isInside ? ngf::Colors::White : ngf::Colors::Red;
        });
      }
      udatePath();
    } else if (event.type == ngf::EventType::MouseButtonPressed) {
      m_from = getRenderTarget()->mapPixelToCoords(event.mouseButton.position);
    }
  }

  void udatePath() {
    const auto path = m_pathFinder.calculatePath(m_from, m_worldPos);
    m_path.clear();
    m_path.reserve(path.size());
    std::transform(path.begin(), path.end(), std::back_inserter(m_path), [](auto pt) {
      return ngf::Vertex{.pos={pt.x, pt.y}};
    });
  }

  void onRender(ngf::RenderTarget &target) override {
    target.clear(ngf::Colors::Lightblue);
    target.draw(ngf::PrimitiveType::LineLoop, m_vertices.data(), m_vertices.size());
    target.draw(ngf::PrimitiveType::LineStrip, m_path.data(), m_path.size());
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Mouse pos (%d,%d)", m_mousePos.x, m_mousePos.y);
    ImGui::Text("World pos (%.f,%.f)", m_worldPos.x, m_worldPos.y);
    auto closestPoint = m_walkbox.getClosestPointOnEdge(m_worldPos);
    ImGui::Text("Closest point (%.1f,%.1f)", closestPoint.x, closestPoint.y);
    ImGui::End();
  }

private:
  std::vector<ngf::Vertex> m_vertices;
  ngf::Walkbox m_walkbox;
  std::vector<ngf::Walkbox> m_walkboxes;
  ngf::PathFinder m_pathFinder;
  std::vector<ngf::Vertex> m_path;
  glm::ivec2 m_mousePos{};
  glm::vec2 m_worldPos{};
  glm::vec2 m_from{22, 72};
  bool m_isInside{false};
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}