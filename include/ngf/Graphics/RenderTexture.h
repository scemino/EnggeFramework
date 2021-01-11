#pragma once
#include <glm/vec2.hpp>
#include <ngf/Graphics/RenderTarget.h>
#include <ngf/Graphics/Texture.h>

namespace ngf {
/// @brief Target for off-screen 2D rendering into a texture.
class RenderTexture : public RenderTarget {
public:
  /// @brief Creates the render-texture.
  /// \param size Size of the render-texture.
  explicit RenderTexture(glm::ivec2 size);

  /// @brief Destructor of the render-texture.
  ~RenderTexture() override;

  /// @brief Gets the size of the render-texture.
  /// \return The size of the render-texture.
  [[nodiscard]] glm::ivec2 getSize() const override { return m_texture.getSize(); }

  /// @brief Gets the target texture.
  [[nodiscard]] const Texture &getTexture() const { return m_texture; }

  /// @brief Activates the render-texture for rendering.
  void activate() override;

  /// @brief Updates the content of the target texture.
  void display();

  /// @brief Captures the window.
  /// \return The image captured.
  [[nodiscard]] Image capture() const { return captureFramebuffer(m_handle); }

protected:
  [[nodiscard]] unsigned int getHandle() const override { return m_handle; }

private:
  unsigned int m_handle{0};
  Texture m_texture;
};
}
