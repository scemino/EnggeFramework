#pragma once
#include <cstdint>
#include <ngf/System/TimeSpan.h>
#include <ngf/Window/Scancode.h>
#include <glm/vec2.hpp>

namespace ngf {
enum class EventType {
  // Application events
  Quit, ///< User has requested to quit

  // Window events
  WindowResized,  //< Window was resized
  WindowClosed,   //< The window manager requests that the window be closed

  // Keyboard events
  KeyPressed,  ///< Key was pressed
  KeyReleased, ///< Key was released

  // Mouse events
  MouseMoved,          ///< Mouse moved
  MouseButtonPressed,  ///< Mouse button pressed
  MouseButtonReleased, ///< Mouse button released
  MouseWheelScrolled,  ///< Mouse wheel scrolled

  // Gamepad events
  GamepadAxisMoved,             ///< Gamepad axis moved
  GamepadButtonPressed,         ///< Gamepad button pressed
  GamepadButtonReleased,        ///< Gamepad button released
  GamepadConnected,             ///< A new Gamepad has been connected
  GamepadDisconnected,          ///< A gamepad has been disconnected
  GamepadMappingUpdated,        ///< A gamepad controller mapping was updated
};

struct WindowEvent {
  std::uint32_t windowId; ///< The window id of the event
};

struct ResizeEvent {
  std::uint32_t windowId;  ///< The id of the window that has been resized
  glm::ivec2 size;         ///< The size of the window
};

struct KeyEvent {
  std::uint32_t windowId;   ///< The window id of the event
  std::int32_t keycode;     ///< Keycode of the key
  Scancode scancode;        ///< Scancode of the key
  std::uint16_t modifiers;  ///< Modifiers that are pressed
  bool repeat;              ///< true if the key was repeated
};

struct MouseButtonEvent {
  std::uint32_t windowId;   ///< The window id of the event
  std::uint32_t id;         ///< The mouse id
  std::uint8_t button;      ///< Mouse button
  glm::ivec2 position;      ///< Mouse position
  std::uint8_t clicks;      ///< Number of clicks
};

struct MouseMovedEvent {
  std::uint32_t windowId;   ///< The window id of the event
  glm::ivec2 position;      ///< Mouse position
};

struct MouseWheelEvent {
  std::uint32_t windowId;   ///< The window id of the event
  glm::ivec2 offset;        ///< Offset of the mouse wheel
};

enum class GamepadAxis {
  LeftX,        ///< The left stick X axis
  LeftY,        ///< The left stick Y axis
  RightX,       ///< The right stick X axis
  RightY,       ///< The right stick Y axis
  TriggerLeft,  ///< The left trigger axis
  TriggerRight, ///< The right trigger axis
};

enum class GamepadButton {
  A,            ///< The A button
  B,            ///< The B button
  X,            ///< The X button
  Y,            ///< The Y button
  Back,         ///< The Back button
  Guide,        ///< The Guide button
  Start,        ///< The Start button
  LeftStick,    ///< The left stick button
  RightStick,   ///< The right stick button
  LeftBumper,   ///< The left bumper button
  RightBumper,  ///< The right bumper button
  DPadUp,       ///< The directional pad up button
  DPadDown,     ///< The directional pad down button
  DPadLeft,     ///< The directional pad left button
  DPadRight,    ///< The directional pad right button
};

struct GamepadButtonEvent {
  std::int32_t id;      ///< Id of the gamepad
  GamepadButton button; ///< Button of the gamepad
};

struct GamepadAxisEvent {
  std::int32_t id;    ///< Id of the gamepad
  GamepadAxis axis;   ///< Axis of the gamepad
  std::int16_t value; ///< Value of the axis
};

struct GamepadConnectionEvent {
  std::int32_t id; ///< Id of the gamepad
};

struct GamepadDisconnectionEvent {
  std::int32_t id; ///< Id of the gamepad
};

struct Event {
  EventType type;          ///< Event type
  TimeSpan timestamp;
  union {
    WindowEvent window;                ///< Window event data
    ResizeEvent resize;                ///< Size event data
    KeyEvent key;                      ///< Key event data
    MouseButtonEvent mouseButton;      ///< Mouse button event data
    MouseMovedEvent mouseMoved;        ///< Mouse moved event data
    MouseWheelEvent mouseWheelEvent;   ///< Mouse wheel event data
    GamepadButtonEvent gamepadButton;  ///< Gamepad button event data
    GamepadAxisEvent gamepadAxis;      ///< Gamepad axis event data
    GamepadConnectionEvent gamepadConnection; ///< Gamepad connection event data
    GamepadDisconnectionEvent gamepadDisconnection; ///< Gamepad disconnection event data
  };
};
}