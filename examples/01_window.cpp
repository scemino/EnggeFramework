#include <ngf/Application.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="01_Window", .size={640, 480}});
  }

  void onRender(ngf::RenderTarget& target) override {
    target.clear(m_color);
    Application::onRender(target);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    auto title = m_window.getTitle();
    if (ImGui::InputText("Title", &title)) {
      m_window.setTitle(title);
    }
    auto isVsync = m_window.isVerticalSyncEnabled();
    if (ImGui::Checkbox("Vertical sync.", &isVsync)) {
      m_window.setVerticalSyncEnabled(isVsync);
    }
    auto isFullscreen = m_window.isWindowFullscreen();
    if (ImGui::Checkbox("Fullscreen", &isFullscreen)) {
      m_window.setWindowFullscreen(isFullscreen);
    }
    auto windowResizable = m_window.isWindowResizable();
    if (ImGui::Checkbox("Resizable", &windowResizable)) {
      m_window.setWindowResizable(windowResizable);
    }
    ngf::ImGui::ColorEdit4("Color", &m_color);
    ImGui::End();
  }

private:
  ngf::Color m_color{ngf::Colors::LightBlue};
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}