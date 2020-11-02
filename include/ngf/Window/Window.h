#pragma once
#include <string>
#include <SDL.h>
#include <glm/vec2.hpp>
#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/Colors.h>
#include <ngf/Window/Event.h>

namespace ngf {
/// @brief Represents a configuration to initialize a window.
struct WindowConfig {
  std::string title; ///< Title of the window.
  glm::ivec2 size{1024, 768}; ///< Size of the window.
  bool vSyncEnabled{true}; ///< true to set the vertical synchronization.
  bool fullscreen{false}; ///< true to set the window in fullscreen.
  bool resizable{true}; ///< true to set the window resizable.
};

/// @brief This class is used to manipulate a window.
class Window {
public:
  /// @brief Creates a new window.
  Window();
  /// @brief Destructor of a window.
  ~Window();

  /// Initializes the window with the specified configuration.
  /// \param config
  void init(const WindowConfig &config);

  /// @brief Display on screen what has been rendered to the window so far.
  void display();

  /// @brief Pops the event on top of the event queue, if any, and return it.
  /// \param event Event on top of the event queue.
  /// \return true if an event was returned, otherwise false if the event queue was empty.
  bool pollEvent(Event &event);

  /// Enable or disable vertical synchronization.
  /// \param enabled true to enable vertical synchronization, otherwise false.
  void setVerticalSyncEnabled(bool enabled);
  /// @brief Gets a value indicating whether or not vertical synchronization is enabled.
  /// \return  true if the vertical synchronization is enable, otherwise false.
  [[nodiscard]] bool isVerticalSyncEnabled() const;

  /// Sets the title of the window.
  /// \param title Title of the window.
  void setTitle(const std::string &title);
  /// @brief Gets the title of the window.
  /// \return The title of the window.
  [[nodiscard]] std::string getTitle() const;

  /// @brief Sets whether or not the window is in fullscreen.
  /// \param fullscreen true to set the window to fullscreen, otherwise false.
  void setWindowFullscreen(bool fullscreen);
  /// @brief Gets a value indicating whether or not the window is in fullscreen.
  /// \return true if the window is in fullscreen, otherwise false.
  [[nodiscard]] bool isWindowFullscreen() const;

  /// @brief Sets window resizable.
  /// \param resizable true to allow the window to be resizable.
  void setWindowResizable(bool resizable);
  /// @brief Gets a value indication whether or not the window is resizable.
  /// \return true if the window is resizable, otherwise false.
  [[nodiscard]] bool isWindowResizable() const;

  /// Gets the size of the window in pixels.
  /// \return
  [[nodiscard]] glm::uvec2 getSize() const;
  /// Gets the scale for the size of the window.
  /// \return the scale for the size of the window.
  [[nodiscard]] static float getSizeScale() { return sizeScale; }

private:
  friend class Application;
  SDL_Window *getNativeHandle() { return m_window;}

private:
  inline static float sizeScale{1.0f};
  SDL_Window *m_window{nullptr};
  SDL_GLContext m_glContext{nullptr};
};
}
