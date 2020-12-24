#pragma once
#include <glm/vec2.hpp>

namespace ngf {
class Mouse final {
public:
  enum class Button {
    Left,
    Middle,
    Right
  };

  /// @brief Gets the mouse position.
  /// \return the mouse position.
  static glm::ivec2 getPosition();

  /// @brief Gets a value indicating whether or not a mouse button is pressed.
  /// \param button Button to test.
  /// \return true if the specified button is pressed.
  static bool isButtonPressed(Button button);
};
}