#include <ngf/Application.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({"01_Window", {640, 480}});
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
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
    ImGui::Separator();
    auto isMouseCursorGrabbed = m_window.isMouseCursorGrabbed();
    if (ImGui::Checkbox("Mouse cursor grabbed", &isMouseCursorGrabbed)) {
      m_window.setMouseCursorGrabbed(isMouseCursorGrabbed);
    }
    auto isMouseCursorVisible = m_window.isMouseCursorVisible();
    if (ImGui::Checkbox("Mouse cursor visible", &isMouseCursorVisible)) {
      m_window.setMouseCursorVisible(isMouseCursorVisible);
    }
    const char *cursors =
        "Arrow\0ArrowWait\0Wait\0Text\0Hand\0SizeHorizontal\0SizeVertical\0SizeTopLeftBottomRight\0SizeBottomLeftTopRight\0SizeAll\0Cross\0NotAllowed\0\0";
    if (ImGui::Combo("Change cursor", &m_cursorIndex, cursors)) {
      m_cursor.setType(static_cast<ngf::Cursor::Type>(m_cursorIndex));
      m_window.setMouseCursor(m_cursor);
    }
    ImGui::Separator();
    ngf::ImGui::ColorEdit4("Color", &m_color);
    auto brightness = m_window.getBrightness();
    if (ImGui::DragFloat("Brightness", &brightness, 0.01f, 0.f, 1.0f)) {
      m_window.setBrightness(brightness);
    }
    auto pos = m_window.getPosition();
    if (ImGui::DragInt2("Position", glm::value_ptr(pos))) {
      m_window.setPosition(pos);
    }
    ImGui::Separator();
    ImGui::InputText("Title", &m_title);
    ImGui::InputText("Message", &m_message);
    const char *types = "Info\0Warning\0Error\0\0";
    ImGui::Combo("Type", &m_type, types);
    if (ImGui::Button("Show")) {
      ngf::MessageBoxType type;
      switch (m_type) {
      case 0:type = ngf::MessageBoxType::Info;
        break;
      case 1:type = ngf::MessageBoxType::Warning;
        break;
      case 2:type = ngf::MessageBoxType::Error;
        break;
      }
      showMessageBox(m_title, m_message, type, &m_window);
    }
    if (ImGui::Button("Show custom MessageBox")) {
      auto result = showMessageBox("Question",
                                   "What is your favorite ghost?",
                                   ngf::MessageBoxType::Info,
                                   {"Blinky", "Inky", "Pinky", "Clyde"},
                                   0,
                                   2,
                                   &m_window);
      switch (result) {
      case 0:m_lastResult = "Blinky";
        break;
      case 1:m_lastResult = "Inky";
        break;
      case 2:m_lastResult = "Pinky";
        break;
      case 3:m_lastResult = "Clyde";
        break;
      }
    }
    ImGui::Text("Last result: %s", m_lastResult.c_str());
    ImGui::End();
  }

private:
  ngf::Cursor m_cursor;
  int m_cursorIndex{0};
  ngf::Color m_color{ngf::Colors::LightBlue};
  std::string m_title{"Title"};
  std::string m_message{"Message"};
  std::string m_lastResult;
  int m_type{0};
};

int main(int argc, char* argv[]) {
  for (const auto &display : ngf::Application::getVideoDisplays()) {
    auto bounds = display.getBounds();
    std::cout << display.getId() << ": " << display.getName() << " [" << bounds.getPosition().x << ", "
              << bounds.getPosition().y << ", "
              << bounds.getSize().x << ", " << bounds.getSize().y << ']' << '\n';
    for (const auto &mode : display.getDisplayModes()) {
      std::cout << "  " << mode.size.x << "x" << mode.size.y << ", " << mode.bitsPerPixel << " bpp, "
                << mode.refreshRate << "Hz\n";
    }
  }

  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}