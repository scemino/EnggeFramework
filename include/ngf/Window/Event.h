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
  std::uint32_t which;      ///< The mouse instance id
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
  };
};
}