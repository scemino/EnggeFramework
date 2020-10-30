#pragma once
#include <string_view>
#include <ngf/System/TimeSpan.h>
#include <ngf/Window/Window.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/RenderStates.h>

namespace ngf {
class Application {
public:
  Application();
  virtual ~Application();

  void run();

protected:
  virtual void onInit();
  virtual void onExit();
  virtual void onUpdate(const TimeSpan &elapsed);
  virtual void onRender(ngf::RenderTarget &target);
  virtual void onImGuiRender();
  virtual void onEvent(Event &event);
  RenderTarget *getRenderTarget() { return m_renderTarget.get(); }

private:
  void processEvents();

protected:
  Window m_window;

private:
  std::string m_title;
  bool m_done{false};
  float m_fps{0};
  int m_frames{0};
  std::unique_ptr<RenderTarget> m_renderTarget;
};
}
