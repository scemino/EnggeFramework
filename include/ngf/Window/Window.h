#pragma once
#include <string>
#include <SDL.h>
#include <Color.h>
#include <Colors.h>
#include <glm/ext/vector_int2.hpp>

namespace ngf {
struct WindowConfig {
  std::string title;
  glm::ivec2 size{1024, 768};
  bool vSyncEnabled{true};
  bool fullscreen{false};
  bool resizable{true};
};

class Window {
public:
  Window();
  ~Window();

  void init(const WindowConfig &config);
  void clear(const Color &color = Colors::Black);
  void display();
  bool pollEvent(SDL_Event &event);

  void setVerticalSyncEnabled(bool enabled);
  [[nodiscard]] bool isVerticalSyncEnabled() const;

  void setTitle(const std::string& title);
  [[nodiscard]] std::string getTitle() const;

  void setWindowFullscreen(bool fullscreen);
  [[nodiscard]] bool isWindowFullscreen() const;

  void setWindowResizable(bool resizable);
  [[nodiscard]] bool isWindowResizable() const;

  SDL_Window *getNativeHandle() {
    return m_window;
  }

private:
  SDL_Window *m_window{nullptr};
  SDL_GLContext m_glContext{nullptr};
};
}
