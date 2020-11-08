#pragma once
#include <ngf/System/TimeSpan.h>
#include <ngf/Graphics/Drawable.h>
#include <ngf/Graphics/Sprite.h>

namespace ngf {
class Animation;

/// @brief An AnimatedSprite is a sprite with an animation.
/// @sa ngf::Sprite, ngf::Animation
class AnimatedSprite : public Drawable {
public:
  /// @brief Sets the animation to use to animate the sprite.
  /// \param animation Animation to use to animate the sprite.
  void setAnimation(Animation *animation);

  /// @brief Updates the animation with the specified elapsed time.
  /// \param elapsed This the time elapsed since last time it has been called.
  void update(const TimeSpan &elapsed);

  /// @brief Draws the sprite to the target with the specified render states.
  /// \param target This is where the drawing is made (a window, a texture, etc.)
  /// \param states Render states to use to draw this sprite.
  void draw(RenderTarget &target, RenderStates states = {}) const override;

  /// @brief Gets the sprite to animate.
  /// \return The sprite to animate.
  Sprite &getSprite() { return m_sprite; }

  /// @brief Gets the sprite to animate.
  /// \return The sprite to animate.
  const Sprite &getSprite() const { return m_sprite; }

private:
  Sprite m_sprite;
  TimeSpan m_elapsed{};
  size_t m_index{0};
  Animation *m_animation{nullptr};
};
}