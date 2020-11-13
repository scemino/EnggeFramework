#pragma once
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/RenderStates.h>

namespace ngf {
/// @brief Abstract base class for objects that can be drawn.
class Drawable {
public:
  /// @brief Destructor of a drawable object.
  virtual ~Drawable() = default;
  /// @brief Draws this object to the target with the specified render states.
  /// \param target This is where the drawing is made (a window, a texture, etc.)
  /// \param states Render states to use to draw this object.
  virtual void draw(RenderTarget &target, RenderStates states) const = 0;
};
}
