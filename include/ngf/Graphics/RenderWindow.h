#pragma once
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/System/Window.h>
#include <glm/vec2.hpp>

namespace ngf {
class RenderWindow final : public RenderTarget {
public:
  /// @brief Creates a render window from a window.
  /// \param window Window where to render.
  explicit RenderWindow(Window &window)
      : RenderTarget(window.getFramebufferSize()), m_window(window) {
  }

  /// @brief Destructor.
  ~RenderWindow() override = default;

  [[nodiscard]] glm::ivec2 getSize() const override { return m_window.getFramebufferSize(); }

  /// @brief Activates the window for rendering.
  void activate() override { m_window.activate(); }

  /// @brief Updates the content of the window.
  void display() { m_window.display(); }

  /// @brief Captures the window.
  /// \return The image captured.
  [[nodiscard]] Image capture() const { return captureFramebuffer(0); }

protected:
  [[nodiscard]] unsigned int getHandle() const override { return 0; }

private:
  Window &m_window;
};
}
