#include <ngf/Application.h>
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>
#include <iostream>
#include <sstream>

namespace {
SDL_GameControllerButton getSdlButton(ngf::GamepadButton button) {
  switch (button) {
  case ngf::GamepadButton::A:return SDL_CONTROLLER_BUTTON_A;
  case ngf::GamepadButton::B:return SDL_CONTROLLER_BUTTON_B;
  case ngf::GamepadButton::X:return SDL_CONTROLLER_BUTTON_X;
  case ngf::GamepadButton::Y:return SDL_CONTROLLER_BUTTON_Y;
  case ngf::GamepadButton::Back:return SDL_CONTROLLER_BUTTON_BACK;
  case ngf::GamepadButton::Guide:return SDL_CONTROLLER_BUTTON_GUIDE;
  case ngf::GamepadButton::Start:return SDL_CONTROLLER_BUTTON_START;
  case ngf::GamepadButton::LeftStick:return SDL_CONTROLLER_BUTTON_LEFTSTICK;
  case ngf::GamepadButton::RightStick:return SDL_CONTROLLER_BUTTON_RIGHTSTICK;
  case ngf::GamepadButton::LeftBumper:return SDL_CONTROLLER_BUTTON_LEFTSHOULDER;
  case ngf::GamepadButton::RightBumper:return SDL_CONTROLLER_BUTTON_RIGHTSHOULDER;
  case ngf::GamepadButton::DPadUp:return SDL_CONTROLLER_BUTTON_DPAD_UP;
  case ngf::GamepadButton::DPadDown:return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
  case ngf::GamepadButton::DPadLeft:return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
  case ngf::GamepadButton::DPadRight:return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
  }

  assert(false);
}

SDL_GameControllerAxis getSdlAxis(ngf::GamepadAxis axis) {
  switch (axis) {
  case ngf::GamepadAxis::LeftX:return SDL_CONTROLLER_AXIS_LEFTX;
  case ngf::GamepadAxis::LeftY:return SDL_CONTROLLER_AXIS_LEFTY;
  case ngf::GamepadAxis::RightX:return SDL_CONTROLLER_AXIS_RIGHTX;
  case ngf::GamepadAxis::RightY:return SDL_CONTROLLER_AXIS_RIGHTY;
  case ngf::GamepadAxis::TriggerLeft:return SDL_CONTROLLER_AXIS_TRIGGERLEFT;
  case ngf::GamepadAxis::TriggerRight:return SDL_CONTROLLER_AXIS_TRIGGERRIGHT;
    assert(false);
  }
}

const char *getButtonName(ngf::GamepadButton button) {
  return SDL_GameControllerGetStringForButton(getSdlButton(button));
}

const char *getAxisName(ngf::GamepadAxis axis) {
  return SDL_GameControllerGetStringForAxis(getSdlAxis(axis));
}

std::ostream &operator<<(std::ostream &o, const ngf::KeyEvent &event) {
  o << "keycode: " << event.keycode << '\n';
  o << "scancode: " << static_cast<int32_t>(event.scancode) << '\n';
  o << "modifiers: " << event.modifiers << '\n';
  o << "repeat: " << (event.repeat ? "yes" : "no");
  return o;
}

std::ostream &operator<<(std::ostream &o, const glm::ivec2 &pos) {
  o << "x=" << pos.x << ',' << pos.y;
  return o;
}

std::ostream &operator<<(std::ostream &o, const ngf::MouseButtonEvent &event) {
  o << "windowId: " << event.windowId << '\n';
  o << "id: " << event.id << '\n';
  o << "button: " << static_cast<int32_t>(event.button) << '\n';
  o << "pos: " << event.position << '\n';
  o << "clicks: " << static_cast<int32_t>(event.clicks);
  return o;
}

std::ostream &operator<<(std::ostream &o, const ngf::MouseMovedEvent &event) {
  o << "windowId: " << event.windowId << '\n';
  o << "pos: " << event.position << '\n';
  return o;
}

std::ostream &operator<<(std::ostream &o, const ngf::GamepadButtonEvent &event) {
  o << "id: " << static_cast<int32_t>(event.id) << '\n';
  o << "button: " << getButtonName(event.button);
  return o;
}

std::ostream &operator<<(std::ostream &o, const ngf::GamepadAxisEvent &event) {
  o << "id: " << static_cast<int32_t>(event.id) << '\n';
  o << "axis: " << getAxisName(event.axis) << '\n';
  o << "value: " << event.value;
  return o;
}

std::ostream &operator<<(std::ostream &o, const ngf::Event &event) {
  o << "timespan: " << event.timestamp.getTotalMilliseconds() << '\n';
  switch (event.type) {
  case ngf::EventType::Quit:o << "type: Quit";
    break;
  case ngf::EventType::WindowResized:o << "type: WindowResized";
    break;
  case ngf::EventType::WindowClosed: o << "type: WindowClosed";
    break;
  case ngf::EventType::MouseMoved:o << "type: MouseMoved" << '\n' << event.mouseMoved;
    break;
  case ngf::EventType::MouseButtonPressed:o << "type: MouseButtonPressed" << '\n' << event.mouseButton;
    break;
  case ngf::EventType::MouseButtonReleased:o << "type: MouseButtonReleased" << '\n' << event.mouseButton;
    break;
  case ngf::EventType::MouseWheelScrolled:o << "type: MouseWheelScrolled";
    break;
  case ngf::EventType::KeyPressed:o << "type: KeyPressed " << '\n' << event.key;
    break;
  case ngf::EventType::KeyReleased:o << "type: KeyReleased " << '\n' << event.key;
    break;
  case ngf::EventType::GamepadConnected:o << "type: GamepadConnected" << '\n' << event.gamepadConnection.id;
    break;
  case ngf::EventType::GamepadDisconnected:o << "type: GamepadDisconnected" << '\n' << event.gamepadDisconnection.id;
    break;
  case ngf::EventType::GamepadButtonPressed:o << "type: GamepadButtonPressed" << '\n' << event.gamepadButton;
    break;
  case ngf::EventType::GamepadButtonReleased:o << "type: GamepadButtonReleased" << '\n' << event.gamepadButton;
    break;
  case ngf::EventType::GamepadAxisMoved:o << "type: GamepadAxisMoved " << '\n' << event.gamepadAxis;
    break;
  case ngf::EventType::GamepadMappingUpdated:o << "type: GamepadMappingUpdated";
    break;
  }
  return o;
}
}

class DemoApplication final : public ngf::Application {
private:
  void onInit() override {
    m_window.init({"02_Inputs", {640, 480}});
  }

  void onRender(ngf::RenderTarget& target) override {
    target.clear(ngf::Colors::LightBlue);
    Application::onRender(target);
  }

  void onEvent(ngf::Event &event) override {
    std::ostringstream o;
    o << event;
    m_lastInput = o.str();
    Application::onEvent(event);
  }

  void onImGuiRender() override {
    ImGui::Begin("Tools");
    ImGui::InputTextMultiline("Last input", &m_lastInput, ImVec2(), ImGuiInputTextFlags_ReadOnly);
    ImGui::End();
  }

private:
  std::string m_lastInput;
};

int main(int argc, char* argv[]) {
  DemoApplication app{};
  app.run();
  return EXIT_SUCCESS;
}