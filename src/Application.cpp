#include <Application.h>
#include <System/StopWatch.h>
#include <imgui.h>
#include <GL/glew.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_sdl.h>

namespace ngf {

namespace {
const TimeSpan TimePerFrame = TimeSpan::seconds(1.f / 60.f);
}

Application::Application() = default;
Application::~Application() = default;

void Application::run() {
  onInit();
  m_renderTarget = std::make_unique<ngf::RenderTarget>();
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

void Application::onRender(ngf::RenderTarget&, const RenderStates&) {
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
}
