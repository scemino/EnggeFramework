#include <ngf/Application.h>
#include <ngf/Graphics/RenderWindow.h>
#include <ngf/System/StopWatch.h>
#include <SDL2/SDL.h>
#include <imgui.h>
#include <GL/glew.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_sdl.h>
#include "src/System/SdlSystem.h"

namespace ngf {

namespace {
const TimeSpan TimePerFrame = TimeSpan::seconds(1.f / 60.f);

SDL_MessageBoxFlags toMessageBoxFlags(MessageBoxType type) {
  switch (type) {
  case MessageBoxType::Error:return SDL_MESSAGEBOX_ERROR;
  case MessageBoxType::Warning:return SDL_MESSAGEBOX_WARNING;
  case MessageBoxType::Info:return SDL_MESSAGEBOX_INFORMATION;
  default:return static_cast<SDL_MessageBoxFlags>(0);
  }
}
}

Application::Application() = default;
Application::~Application() = default;

void Application::run() {
  onInit();
  m_renderTarget = std::make_unique<ngf::RenderWindow>(m_window);
  processEvents();

  int frames = 0;
  StopWatch fpsStopWatch;
  StopWatch stopWatch;
  auto timeSinceLastUpdate = TimeSpan::Zero;
  // Main loop
  while (!m_done) {
    auto elapsed = stopWatch.restart();
    timeSinceLastUpdate += elapsed;
    while (timeSinceLastUpdate > TimePerFrame) {
      timeSinceLastUpdate -= TimePerFrame;
      processEvents();
      onUpdate(TimePerFrame);
    }

    if (fpsStopWatch.getElapsedTime() >= TimeSpan::seconds(1)) {
      m_fps = static_cast<float>(frames) / fpsStopWatch.getElapsedTime().getTotalSeconds();
      fpsStopWatch.restart();
      frames = 0;
    }

    onRender(*m_renderTarget);
    frames++;
  }

  onExit();
}

void Application::processEvents() {
  Event event;
  while (m_window.pollEvent(event)) {
    if (event.type == EventType::Quit) {
      m_done = true;
    } else {
      onEvent(event);
    }
  }
}

void Application::onInit() {
  m_window.init({});
}

void Application::onExit() {
}

void Application::onUpdate(const TimeSpan &) {
}

void Application::onRender(ngf::RenderTarget &) {
  // Render dear imgui
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(m_window.getNativeHandle());
  ImGui::NewFrame();

  onImGuiRender();

  // imgui render
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  m_frames++;
  m_window.display();
}

void Application::onImGuiRender() {
}

void Application::onEvent(Event &) {
}

void Application::quit() {
  m_done = true;
}

void Application::showMessageBox(const std::string &title,
                                 const std::string &message,
                                 MessageBoxType type,
                                 Window *window) {
  auto handle = window ? window->getNativeHandle() : nullptr;
  SDL_ShowSimpleMessageBox(toMessageBoxFlags(type), title.c_str(), message.c_str(), handle);
}

int Application::showMessageBox(const std::string &title,
                                const std::string &message,
                                MessageBoxType type,
                                std::initializer_list<std::string> buttons,
                                int acceptButton, int cancelButton,
                                Window *window) {
  auto handle = window ? window->getNativeHandle() : nullptr;

  // convert buttons
  int i = 0;
  std::vector<SDL_MessageBoxButtonData> buttonsData;
  for (const auto &button : buttons) {
      Uint32 buttonFlags;
    if (acceptButton == i)
      buttonFlags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
    else if (cancelButton == i)
      buttonFlags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
    else
      buttonFlags = 0;
    buttonsData.push_back({buttonFlags, i++, button.c_str()});
  }

  SDL_MessageBoxData data;
  data.window = handle;
  data.message = message.c_str();
  data.title = title.c_str();
  data.flags = toMessageBoxFlags(type);
  data.numbuttons = buttonsData.size();
  data.buttons = buttonsData.data();
  data.colorScheme = nullptr;

  int id;
  SDL_ShowMessageBox(&data, &id);
  return id;
}

VideoDisplayIterator Application::getVideoDisplays() {
  priv::SdlSystem::ensureInit();
  return VideoDisplayIterator();
}
}
