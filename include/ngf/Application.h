#pragma once
#include <string_view>
#include "System/TimeSpan.h"
#include "Window/Window.h"

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
  virtual void onRender();
  virtual void onImGuiRender();
  virtual void onEvent(SDL_Event &event);

private:
  void processEvents();

protected:
  std::string m_title;
  Window m_window;
  bool m_done{false};
  float m_fps{0};
  int m_frames{0};
};
}
