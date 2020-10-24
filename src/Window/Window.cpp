#include <ngf/Window/Window.h>
#include <GL/glew.h>
#include <SDL.h>
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_sdl.h>
#include <iostream>
#include <sstream>

namespace ngf {

namespace {
GamepadButton getGamepadButton(Uint8 button) {
  switch (button) {
  case SDL_CONTROLLER_BUTTON_A:return GamepadButton::A;
  case SDL_CONTROLLER_BUTTON_B:return GamepadButton::B;
  case SDL_CONTROLLER_BUTTON_X:return GamepadButton::X;
  case SDL_CONTROLLER_BUTTON_Y:return GamepadButton::Y;
  case SDL_CONTROLLER_BUTTON_BACK:return GamepadButton::Back;
  case SDL_CONTROLLER_BUTTON_GUIDE:return GamepadButton::Guide;
  case SDL_CONTROLLER_BUTTON_START:return GamepadButton::Start;
  case SDL_CONTROLLER_BUTTON_LEFTSTICK:return GamepadButton::LeftStick;
  case SDL_CONTROLLER_BUTTON_RIGHTSTICK:return GamepadButton::RightStick;
  case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:return GamepadButton::LeftBumper;
  case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:return GamepadButton::RightBumper;
  case SDL_CONTROLLER_BUTTON_DPAD_UP:return GamepadButton::DPadUp;
  case SDL_CONTROLLER_BUTTON_DPAD_DOWN:return GamepadButton::DPadDown;
  case SDL_CONTROLLER_BUTTON_DPAD_LEFT:return GamepadButton::DPadLeft;
  case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:return GamepadButton::DPadRight;
  }
  assert(false);
}

GamepadAxis getGamepadAxis(Uint8 axis) {
  switch (axis) {
  case SDL_CONTROLLER_AXIS_LEFTX:return GamepadAxis::LeftX;
  case SDL_CONTROLLER_AXIS_LEFTY:return GamepadAxis::LeftY;
  case SDL_CONTROLLER_AXIS_RIGHTX:return GamepadAxis::RightX;
  case SDL_CONTROLLER_AXIS_RIGHTY:return GamepadAxis::RightY;
  case SDL_CONTROLLER_AXIS_TRIGGERLEFT:return GamepadAxis::TriggerLeft;
  case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:return GamepadAxis::TriggerRight;
  }

  assert(false);
}
}

Window::Window() = default;

Window::~Window() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  SDL_GL_DeleteContext(m_glContext);
  SDL_DestroyWindow(m_window);
  SDL_Quit();
}

void Window::init(const WindowConfig &config) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
    std::ostringstream ss;
    ss << "Error when initializing SDL (error=" << SDL_GetError() << ")";
    throw std::runtime_error(ss.str());
  }

  // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.2 Core + GLSL 150
  const char *glsl_version = "#version 150";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS,
                      SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);// Always required on Mac
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
  SDL_GL_SetAttribute(
      SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

  // Create window with graphics context
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  auto window_flags = (SDL_WindowFlags) (
      SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI |
          (config.resizable ? SDL_WINDOW_RESIZABLE : 0) |
          (config.fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
  m_window = SDL_CreateWindow(config.title.data(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, config.size.x, config.size.y, window_flags);

  // setup OpenGL
  m_glContext = SDL_GL_CreateContext(m_window);
  if (!m_glContext) {
    std::ostringstream ss;
    ss << "Error when creating GL context (error=" << SDL_GetError() << ")";
    throw std::runtime_error(ss.str());
  }

  SDL_GL_MakeCurrent(m_window, m_glContext);
  setVerticalSyncEnabled(config.vSyncEnabled);

  auto err = glGetError();
  if (err != GL_NO_ERROR) {
    std::ostringstream ss;
    ss << "Error when initializing OpenGL " << SDL_GetError();
    throw std::runtime_error(ss.str());
  };

  err = glewInit();
  if (err != GLEW_OK) {
    std::ostringstream ss;
    ss << "Error when initializing glew " << glewGetErrorString(err);
    throw std::runtime_error(ss.str());
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  // window is the SDL_Window*
  // contex is the SDL_GLContext
  ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void Window::display() {
  SDL_GL_SwapWindow(m_window);
}

bool Window::pollEvent(Event &event) {
  SDL_Event sdlEvent;
  if (!SDL_PollEvent(&sdlEvent))
    return false;

  ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
  if (ImGui::GetIO().WantTextInput || ImGui::GetIO().WantCaptureMouse) {
    return false;
  }

  event.timestamp = TimeSpan::milliseconds(sdlEvent.common.timestamp);

  switch (sdlEvent.type) {
  case SDL_QUIT:event.type = EventType::Quit;
    break;
  case SDL_WINDOWEVENT:
    switch (sdlEvent.window.event) {
    case SDL_WINDOWEVENT_RESIZED:
    case SDL_WINDOWEVENT_SIZE_CHANGED:event.type = EventType::WindowResized;
      event.resize.windowId = sdlEvent.window.windowID;
      event.resize.size = glm::ivec2{sdlEvent.window.data1, sdlEvent.window.data2};
      break;
    case SDL_WINDOWEVENT_CLOSE:event.type = EventType::WindowClosed;
      event.window.windowId = sdlEvent.window.windowID;
      break;
    default: return false;
    }
  case SDL_KEYDOWN:
  case SDL_KEYUP:event.type = sdlEvent.type == SDL_KEYDOWN ? EventType::KeyPressed : EventType::KeyReleased;
    event.key.windowId = sdlEvent.key.windowID;
    event.key.keycode = static_cast<std::int32_t>(sdlEvent.key.keysym.sym);
    event.key.scancode = static_cast<Scancode>(sdlEvent.key.keysym.scancode);
    event.key.modifiers = sdlEvent.key.keysym.mod;
    event.key.repeat = sdlEvent.key.repeat != 0;
    break;
  case SDL_MOUSEBUTTONDOWN:
  case SDL_MOUSEBUTTONUP:
    event.type = sdlEvent.type == SDL_MOUSEBUTTONDOWN ? EventType::MouseButtonPressed
                                                      : EventType::MouseButtonReleased;
    event.mouseButton.windowId = sdlEvent.button.windowID;
    event.mouseButton.id = sdlEvent.button.which;
    event.mouseButton.button = static_cast<std::int32_t>(sdlEvent.button.button);
    event.mouseButton.position = glm::ivec2{sdlEvent.button.x, sdlEvent.button.y};
    event.mouseButton.clicks = sdlEvent.button.clicks;
    break;
  case SDL_MOUSEWHEEL:event.type = EventType::MouseWheelScrolled;
    event.mouseWheelEvent.windowId = sdlEvent.wheel.windowID;
    event.mouseWheelEvent.offset = glm::ivec2{sdlEvent.wheel.x, sdlEvent.wheel.x};
    break;
  case SDL_MOUSEMOTION:event.type = EventType::MouseMoved;
    event.mouseMoved.windowId = sdlEvent.motion.windowID;
    event.mouseMoved.position = glm::ivec2{sdlEvent.motion.x, sdlEvent.motion.y};
    break;
  case SDL_CONTROLLERBUTTONDOWN:
  case SDL_CONTROLLERBUTTONUP:
    event.type = sdlEvent.type == SDL_CONTROLLERBUTTONDOWN ? EventType::GamepadButtonPressed
                                                           : EventType::GamepadButtonReleased;
    event.gamepadButton.id = static_cast<std::int32_t>(sdlEvent.cbutton.which);
    event.gamepadButton.button = getGamepadButton(sdlEvent.cbutton.button);
    break;
  case SDL_CONTROLLERDEVICEADDED: {
    event.type = EventType::GamepadConnected;
    event.gamepadConnection.id = static_cast<std::int32_t>(sdlEvent.cdevice.which);
    auto *controller = SDL_GameControllerOpen(event.gamepadConnection.id);
    if (!controller) {
      std::cerr << "Could not open gamepad " << event.gamepadConnection.id << ": " << SDL_GetError() << "\n";
    }
  }
    break;
  case SDL_CONTROLLERDEVICEREMOVED: {
    event.type = EventType::GamepadDisconnected;
    auto controllerId = SDL_GameControllerFromInstanceID(sdlEvent.cdevice.which);
    event.gamepadDisconnection.id = static_cast<std::int32_t>(sdlEvent.cdevice.which);
    if (controllerId) {
      SDL_GameControllerClose(controllerId);
    }
  }
    break;
  case SDL_CONTROLLERAXISMOTION:event.type = EventType::GamepadAxisMoved;
    event.gamepadAxis.id = static_cast<std::int32_t>(sdlEvent.caxis.which);
    event.gamepadAxis.axis = getGamepadAxis(sdlEvent.caxis.axis);
    event.gamepadAxis.value = sdlEvent.caxis.value;
    break;
  default: return false;
  }
  return true;
}

void Window::setVerticalSyncEnabled(bool enabled) {
  SDL_GL_SetSwapInterval(enabled ? 1 : 0);
}

bool Window::isVerticalSyncEnabled() const {
  return SDL_GL_GetSwapInterval();
}

void Window::setTitle(const std::string &title) {
  SDL_SetWindowTitle(m_window, title.data());
}

std::string Window::getTitle() const {
  return SDL_GetWindowTitle(m_window);
}

void Window::setWindowFullscreen(bool fullscreen) {
  SDL_SetWindowFullscreen(m_window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

bool Window::isWindowFullscreen() const {
  auto flags = SDL_GetWindowFlags(m_window);
  return (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;
}

void Window::setWindowResizable(bool resizable) {
  SDL_SetWindowResizable(m_window, resizable ? SDL_TRUE : SDL_FALSE);
}

bool Window::isWindowResizable() const {
  auto flags = SDL_GetWindowFlags(m_window);
  return (flags & SDL_WINDOW_RESIZABLE) != 0;
}

glm::uvec2 Window::getSize() const {
  int width;
  int height;
  SDL_GetWindowSize(m_window, &width, &height);
  return glm::uvec2(width, height);
}

}
