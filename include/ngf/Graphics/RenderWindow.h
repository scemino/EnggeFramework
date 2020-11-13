#pragma once
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/System/Window.h>
#include <glm/vec2.hpp>

namespace ngf {
class RenderWindow final : public RenderTarget {
public:
  explicit RenderWindow(Window &window)
      : RenderTarget(window.getFramebufferSize()), m_window(window) {
  }

  [[nodiscard]] glm::ivec2 getSize() const override { return m_window.getFramebufferSize(); }

  /// @brief Activates the window for rendering.
  void setActive() override { m_window.setActive(); }

  /// @brief Updates the content of the window.
  void display() { m_window.display(); }

private:
  Window &m_window;
};
}
