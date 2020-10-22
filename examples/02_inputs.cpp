#include <ngf/Application.h>
#include <ngf/Graphics/ImGuiExtensions.h>
#include <imgui.h>
#include <imgui_stdlib.h>

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({.title="02_Inputs", .size={640, 480}});
  }

  void onRender() override {
    m_window.clear(ngf::Colors::Lightblue);
    Application::onRender();
  }

  void onEvent(ngf::Event &event) override {
    m_lastInput.clear();
    m_lastInput.append(std::to_string(event.timestamp.getTotalMilliseconds())).append(": ");
    switch (event.type) {
    case ngf::EventType::Quit:m_lastInput.append("Quit");
      break;
    case ngf::EventType::WindowResized:m_lastInput.append("WindowResized");
      break;
    case ngf::EventType::WindowClosed:m_lastInput.append("WindowClosed");
      break;
    case ngf::EventType::MouseMoved:
      m_lastInput.append("MouseMoved (x=")
          .append(std::to_string(event.mouseMoved.position.x))
          .append(" y=").append(std::to_string(event.mouseMoved.position.y))
          .append(")");
      break;
    case ngf::EventType::MouseButtonPressed:m_lastInput.append("MouseButtonPressed ").append(std::to_string(event.mouseButton.button));
      break;
    case ngf::EventType::MouseButtonReleased:m_lastInput.append("MouseButtonReleased ").append(std::to_string(event.mouseButton.button));
      break;
    case ngf::EventType::MouseWheelScrolled:m_lastInput.append("MouseWheelScrolled");
      break;
    case ngf::EventType::KeyPressed:
      m_lastInput.append("KeyPressed ").append(std::to_string(event.key.keycode)).append("[").append(std::to_string(
          event.key.repeat)).append("]");
      break;
    case ngf::EventType::KeyReleased:
      m_lastInput.append("KeyReleased ").append(std::to_string(event.key.keycode)).append("[").append(std::to_string(
          event.key.repeat)).append("]");
      break;
    }
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::LabelText("Last input", "%s", m_lastInput.data());
    ImGui::End();
  }

private:
  std::string m_lastInput;
};

int main() {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}