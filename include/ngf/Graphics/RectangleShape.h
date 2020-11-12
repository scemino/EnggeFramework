#include <ngf/Graphics/Shape.h>
#include <glm/vec2.hpp>

namespace ngf {
class RectangleShape : public Shape {
public:
  /// @brief Creates an empty rectangle shape.
  RectangleShape() = default;
  /// @brief Creates a rectangle shape of a specified size.
  /// \param size Size of the rectangle to create.
  explicit RectangleShape(glm::vec2 size);
  /// @brief Creates a rectangle shape of a specified size and position.
  /// \param rect Rectangle specifying the position and size of the rectangle to create.
  explicit RectangleShape(const ngf::frect &rect);

  /// @brief Sets the size of the rectangle.
  /// \param size Size to set to the rectangle.
  void setSize(glm::vec2 size);

  /// @brief Gets the size of the rectangle.
  /// \return The size of the rectangle.
  glm::vec2 getSize() const { return m_size; }

  /// @brief Gets the number of points composing the rectangle.
  /// \return the number of points composing the rectangle.
  std::size_t getPointCount() const override;

  /// @brief Gets a point composing the rectangle at the specified index.
  /// \param index Index of the point to get.
  /// \return Point composing the rectangle at the specified index.
  glm::vec2 getPoint(std::size_t index) const override;

private:
  glm::vec2 m_size{0.f, 0.f};
};
}