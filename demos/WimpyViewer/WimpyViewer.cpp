#include <ngf/Application.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/Vertex.h>
#include <imgui.h>
#include <sstream>
#include <utility>
#include "SpriteSheetItem.h"
#include "Camera.h"
#include "Room.h"

class WimpyViewerApplication final : public ngf::Application {
public:
  explicit WimpyViewerApplication(std::filesystem::path path)
      : m_path(std::move(path)) {
  }

private:
  void onInit() override {
    m_window.init({.title="WimpyViewer", .size={1024, 768}});
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
    target.clear(ngf::Colors::LightBlue);

    ngf::RenderStates states;
    m_room.draw(target, states);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::Text("%.2f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Text("Mouse pos (%d,%d)", m_mousePos.x, m_mousePos.y);
    ImGui::Text("World pos (%.f,%.f)", m_worldPos.x, m_worldPos.y);

    ImGui::DragFloat2("Position", glm::value_ptr(m_room.getCamera().position));
    ImGui::DragFloat("Zoom", &m_zoom, 0.1f, 0.1f, 10.f);

    int i = 1;
    for (const auto &layer : m_room) {
      auto zsort = layer->getZSort();
      std::ostringstream s;
      s << "Background";
      if (zsort) {
        s << ' ' << zsort;
      }
      if (ImGui::TreeNode(s.str().c_str())) {
        auto visible = layer->isVisible();
        if (ImGui::Checkbox("Visible", &visible)) {
          layer->setVisible(visible);
        }
        auto parallax = layer->getParallax();
        if (ImGui::DragFloat2("Parallax", glm::value_ptr(parallax))) {
          layer->setParallax(parallax);
        }
        ImGui::TreePop();
      }
    }
    ImGui::End();
  }

private:
  std::filesystem::path m_path;
  Room m_room;
  float m_zoom{1.f};
  glm::ivec2 m_mousePos{};
  glm::vec2 m_worldPos{};
};

int main(int argc, char **argv) {
  if (argc >= 2) {
    WimpyViewerApplication app{argv[1]};
    app.run();
    return EXIT_SUCCESS;
  }
  return EXIT_FAILURE;
}