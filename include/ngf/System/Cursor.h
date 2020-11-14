#pragma once

struct SDL_Cursor;

namespace ngf {
/// @brief Mouse cursor.
class Cursor {
public:
  /// @brief Enumeration of the native system cursor types.
  enum Type {
    Arrow,                  ///< Arrow cursor (default)
    ArrowWait,              ///< Busy arrow cursor
    Wait,                   ///< Busy cursor
    Text,                   ///< I-beam, cursor when hovering over a field allowing text entry
    Hand,                   ///< Pointing hand cursor
    SizeHorizontal,         ///< Horizontal double arrow cursor
    SizeVertical,           ///< Vertical double arrow cursor
    SizeTopLeftBottomRight, ///< Double arrow cursor going from top-left to bottom-right
    SizeBottomLeftTopRight, ///< Double arrow cursor going from bottom-left to top-right
    SizeAll,                ///< Combination of SizeHorizontal and SizeVertical
    Cross,                  ///< Crosshair cursor
    NotAllowed              ///< Action not allowed cursor
  };

private:
  explicit Cursor(SDL_Cursor *cursor);

public:
  /// @brief Creates an empty cursor.
  Cursor() = default;

  /// @brief Creates a cursor from prefined type.
  /// \param type Type of the cursor to create.
  explicit Cursor(Type type);
  
  /// @brief Destructor of a mouse cursor.
  ~Cursor();

  /// @brief Sets the type of cursor.
  /// \param type Type of the cursor to set.
  void setType(Type type);

private:
  friend class Window;
  SDL_Cursor *m_cursor{nullptr};
  bool m_freeCursor{true};
};
}