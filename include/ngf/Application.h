#pragma once
#include <string_view>
#include <ngf/System/TimeSpan.h>
#include <System/Window.h>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/RenderWindow.h>
#include <ngf/Graphics/RenderStates.h>

namespace ngf {
class Application {
public:
  Application();
  virtual ~Application();

  /// @brief Runs the main loop of the application.
  void run();

  /// @brief Quits the application.
  void quit();

  /// @brief Displays a message box with a title, a message, a type and an 'OK' button.
  /// \param title Title of the message box.
  /// \param message Message to display.
  /// \param type Type of the message: Information, Warning or Error.
  /// \param window Parent window or nullptr if no Window is available.
  static void showMessageBox(const std::string &title,
                             const std::string &message,
                             MessageBoxType type = MessageBoxType::Info,
                             Window *window = nullptr);

  /// @brief Displays a message box with a title, a message, a type and a list of buttons.
  /// \param title Title of the message box.
  /// \param message Message to display.
  /// \param type Type of the message: Information, Warning or Error.
  /// \param buttons List of button texts.
  /// \param acceptButton Index of the accept button or -1.
  /// \param cancelButton Index of the cancel button or -1.
  /// \param window Parent window or nullptr if no Window is available.
  /// \return The index of the button which has been pressed.
  [[nodiscard]] static int showMessageBox(const std::string &title, const std::string &message, MessageBoxType type,
                                          std::initializer_list<std::string> buttons,
                                          int acceptButton = -1, int cancelButton = -1,
                                          Window *window = nullptr);

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
  std::unique_ptr<RenderWindow> m_renderTarget;
};
}
