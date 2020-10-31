#pragma once
#include <glm/vec2.hpp>
#include <glm/mat3x3.hpp>
#include <ngf/Graphics/Rect.h>

namespace ngf {
/// @brief A view defines what region is displayed.
class View {
public:
  /// @brief Creates an empty view.
  View();
  /// @brief Creates a view from a specified rectangle.
  /// \param rect The rectangle defining the region to display.
  explicit View(const frect &rect);
  /// Creates a view from a center and a size.
  /// \param center The center of the view to create.
  /// \param size The size of the view to create.
  View(const glm::vec2& center, const glm::vec2& size);

  /// @brief Gets the bounds of the view.
  /// \return The bounds of the view.
  [[nodiscard]] frect getBounds() const;

  /// @brief Sets the center of the view.
  /// \param center The center of the view.
  void setCenter(glm::vec2 center) { m_center = center; }
  /// @brief Gets the center of the view.
  /// \return The center of the view.
  [[nodiscard]] glm::vec2 getCenter() const { return m_center; }

  /// @brief Sets the size of the view.
  /// \param center The size of the view.
  void setSize(glm::vec2 size) { m_size = size; }
  /// @brief Gets the size of the view.
  /// \return The size of the view.
  [[nodiscard]] glm::vec2 getSize() const { return m_size; }

  /// Sets the rotation of the view
  /// \param angle Angle in radians.
  void setRotation(float angle) { m_rotation = angle; }
  /// Gets the rotation of the view
  /// \param angle Angle in radians.
  [[nodiscard]] float getRotation() const { return m_rotation; }

  /// @brief Sets the viewport.
  /// \param viewport The viewport to set.
  void setViewport(const frect &viewport);
  /// @brief Gets the viewport.
  /// \return The viewport.
  [[nodiscard]] const frect &getViewport() const { return m_viewport; }

  /// Resets the view to a specified rectangle.
  /// \param rect The rectangle defining the region to display.
  void reset(const frect &rect);
  /// @brief Moves the view relatively to its current position.
  /// \param offset The offset used to move the view.
  void move(const glm::vec2& offset);
  /// @brief Rotates the view relatively to its current rotation.
  /// \param angle Angle to rotate, in radians.
  void rotate(float angle);
  /// @brief  Resizes the view rectangle relatively to its current size.
  /// \param factor Zoom factor to apply.
  void zoom(float factor);

  /// Gets the projection transformation of the view.
  /// \return The projection transformation of the view.
  [[nodiscard]] glm::mat3 getTransform() const;
  /// Gets the inverse projection transform of the view.
  /// \return The inverse projection transform of the view.
  [[nodiscard]] glm::mat3 getInverseTransform() const;

private:
  glm::vec2 m_center;
  glm::vec2 m_size;
  float m_rotation;
  frect m_viewport;
};
}
