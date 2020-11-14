#include <ngf/Application.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="01_Window", .size={640, 480}});
  }

  void onRender(ngf::RenderTarget &target) override {
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
    auto isFullscreen = m_window.isFullscreen();
    if (ImGui::Checkbox("Fullscreen", &isFullscreen)) {
      m_window.setFullscreen(isFullscreen);
    }
    auto windowResizable = m_window.isWindowResizable();
    if (ImGui::Checkbox("Resizable", &windowResizable)) {
      m_window.setWindowResizable(windowResizable);
    }
    auto isDecorated = m_window.isDecorated();
    if (ImGui::Checkbox("Decorated", &isDecorated)) {
      m_window.setDecorated(isDecorated);
    }
    auto isMaximized = m_window.isMaximized();
    if (ImGui::Checkbox("Maximized", &isMaximized)) {
      if (isMaximized) {
        m_window.maximize();
      } else {
        m_window.restore();
      }
    }
    auto isMinimized = m_window.isMinimized();
    if (ImGui::Checkbox("Minimized", &isMinimized)) {
      if (isMinimized) {
        m_window.minimize();
      } else {
        m_window.restore();
      }
    }
    auto isMouseCursorGrabbed = m_window.isMouseCursorGrabbed();
    if (ImGui::Checkbox("Mouse cursor grabbed", &isMouseCursorGrabbed)) {
      m_window.setMouseCursorGrabbed(isMouseCursorGrabbed);
    }
    auto isMouseCursorVisible = m_window.isMouseCursorVisible();
    if (ImGui::Checkbox("Mouse cursor visible", &isMouseCursorVisible)) {
      m_window.setMouseCursorVisible(isMouseCursorVisible);
    }
    ngf::ImGui::ColorEdit4("Color", &m_color);
    auto brightness = m_window.getBrightness();
    if (ImGui::DragFloat("Brightness", &brightness, 0.01f, 0.f, 1.0f)) {
      m_window.setBrightness(brightness);
    }
    auto pos = m_window.getPosition();
    if (ImGui::DragInt2("Position", glm::value_ptr(pos))) {
      m_window.setPosition(pos);
    }
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