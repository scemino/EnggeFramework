#pragma once
#include <string>
#include <initializer_list>
#include <SDL2/SDL.h>
#include <glm/vec2.hpp>
#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/Colors.h>
#include <ngf/System/Event.h>
#include <ngf/System/Cursor.h>

namespace ngf {
/// @brief Represents a configuration to initialize a window.
struct WindowConfig {
  std::string title; ///< Title of the window.
  glm::ivec2 size{1024, 768}; ///< Size of the window.
  bool vSyncEnabled{true}; ///< true to set the vertical synchronization.
  bool fullscreen{false}; ///< true to set the window in fullscreen.
  bool resizable{true}; ///< true to set the window resizable.
};

enum class MessageBoxType {
  Error,
  Warning,
  Info
};

/// @brief This class is used to manipulate a window.
class Window {
public:
  /// @brief Creates a new window.
  Window();

  /// @brief Destructor of a window.
  ~Window();

  /// @brief Initializes the window with the specified configuration.
  /// \param config
  void init(const WindowConfig &config);

  /// @brief Display on screen what has been rendered to the window so far.
  void display();

  /// @brief Pops the event on top of the event queue, if any, and return it.
  /// \param event Event on top of the event queue.
  /// \return true if an event was returned, otherwise false if the event queue was empty.
  bool pollEvent(Event &event);

  /// @brief Enable or disable vertical synchronization.
  /// \param enabled true to enable vertical synchronization, otherwise false.
  void setVerticalSyncEnabled(bool enabled = true);

  /// @brief Gets a value indicating whether or not vertical synchronization is enabled.
  /// \return  true if the vertical synchronization is enable, otherwise false.
  [[nodiscard]] bool isVerticalSyncEnabled() const;

  /// @brief Sets the title of the window.
  /// \param title Title of the window.
  void setTitle(const std::string &title);

  /// @brief Gets the title of the window.
  /// \return The title of the window.
  [[nodiscard]] std::string getTitle() const;

  /// @brief Sets whether or not the window is in fullscreen.
  /// \param fullscreen true to set the window to fullscreen, otherwise false.
  void setFullscreen(bool fullscreen = true);

  /// @brief Gets a value indicating whether or not the window is in fullscreen.
  /// \return true if the window is in fullscreen, otherwise false.
  [[nodiscard]] bool isFullscreen() const;

  /// @brief Sets window resizable.
  /// \param resizable true to allow the window to be resizable.
  void setWindowResizable(bool resizable = true);

  /// @brief Gets a value indication whether or not the window is resizable.
  /// \return true if the window is resizable, otherwise false.
  [[nodiscard]] bool isWindowResizable() const;

  /// @brief Gets the size of the window in pixels.
  /// \return The size of the window in pixels.
  [[nodiscard]] glm::ivec2 getSize() const;

  /// @brief Sets the size of the window in pixels.
  /// \param size The size of the window in pixels.
  void setSize(const glm::ivec2 &size);

  /// @brief Gets the DPI scale of the window.
  /// \return the DPI scale of the window.
  [[nodiscard]] static float getDpiScale() { return dpiScale; }

  /// @brief Activates the target for rendering.
  void activate();

  /// @brief Gets the size of the underlying framebuffer in pixels.
  /// \return The size of the underlying framebuffer in pixels.
  [[nodiscard]] glm::ivec2 getFramebufferSize() const;

  /// @brief Gets the position of the window on the screen.
  [[nodiscard]] glm::ivec2 getPosition() const;

  /// @brief Sets the position of the window on the screen.
  /// \param pos The position of the window on the screen.
  void setPosition(const glm::ivec2 &pos);

  /// @brief Gets the brightness (gamma multiplier) for the display that owns this window.
  /// \return The brightness (gamma multiplier) value to set where 0.0 is completely dark and 1.0 is normal brightness.
  [[nodiscard]] float getBrightness() const;

  /// @brief Sets the brightness (gamma multiplier) for the display that owns this window.
  /// \param brightness The brightness (gamma multiplier) value to set where 0.0 is completely dark and 1.0 is normal brightness.
  void setBrightness(float brightness);

  /// @brief Gets a value indicating whether or not this window is minimized.
  /// \return true if this window is minimized, false otherwise.
  [[nodiscard]] bool isMinimized() const;

  /// @brief Minimizes this window to an iconic representation.
  void minimize();

  /// @brief Gets a value indicating whether or not this window is maximized.
  /// \return true if this window is maximized, false otherwise.
  [[nodiscard]] bool isMaximized() const;

  /// @brief Makes this window as large as possible.
  void maximize();

  /// @brief Restores this window.
  void restore();

  /// @brief Gets a value indicating whether or not this window is visible.
  /// \return true if this window is visible, false otherwise.
  [[nodiscard]] bool isVisible() const;

  /// @brief Sets the window visible or hidden.
  /// \param visible true to set the window visible, false otherwise.
  void setVisible(bool visible = true);

  /// @brief Gets a value indicating whether or not the window decoration is visible.
  /// \return true if the window decoration is visible, false otherwise.
  [[nodiscard]] bool isDecorated() const;

  /// @brief Sets the window decoration visible or hidden.
  /// \param decorated true to set the window decoration visible, false otherwise.
  void setDecorated(bool decorated = true);

  /// @brief Sets the mouse cursor visible or hidden.
  /// \param decorated true to set the mouse cursor visible, false otherwise.
  void setMouseCursorVisible(bool visible = true);

  /// @brief Gets a value indicating whether or not the mouse cursor is visible.
  /// \return true if the mouse cursor is visible, false otherwise.
  [[nodiscard]] bool isMouseCursorVisible() const;

  /// @brief Sets window's input grab mode.
  /// \param grabbed true to grab input, false to release.
  void setMouseCursorGrabbed(bool grabbed = true);

  /// @brief Gets a value indicating whether or not the mouse cursor is grabbed.
  /// \return true if input is grabbed, false if released.
  [[nodiscard]] bool isMouseCursorGrabbed() const;

  /// @brief Sets the mouse cursor.
  /// \param cursor The mouse cursor to set.
  void setMouseCursor(const Cursor& cursor);

  /// @brief Gets the mouse cursor.
  /// \return The mouse cursor.
  [[nodiscard]] Cursor getMouseCursor() const;

private:
  friend class Application;
  SDL_Window *getNativeHandle() { return m_window; }

private:
  inline static float dpiScale{1.0f};
  SDL_Window *m_window{nullptr};
  SDL_GLContext m_glContext{nullptr};
  unsigned int m_vao{0};
};
}
