#pragma once
#include <cassert>
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <filesystem>
#include "Image.h"

namespace ngf {
/// @brief An image that lives in the graphic memory that can be used for drawing.
class Texture {
public:
  enum class Format {
    Rgba,   ///< RGBA format.
    Rgb,    ///< RGB format.
    Alpha,  ///< Alpha format.
  };

  enum class Type {
    Texture1D,  // 1 dimension texture.
    Texture2D,  // 2 dimensions texture.
  };

  /// @brief Creates a texture from a specifed type and format.
  /// \param type The type of the texture to create.
  /// \param format The format of the texture to create.
  explicit Texture(Type type = Type::Texture2D, Format format = Format::Rgba);

  /// @brief Creates a texture from an image.
  /// \param image Image to use to create the texture.
  explicit Texture(const Image &image);

  /// @brief Creates a texture from a file.
  /// \param path The path of the image file to load.
  explicit Texture(const std::filesystem::path &path);

  /// @brief Creates a 2D texture from a specifed format, size and memory.
  /// \param format The format of the texture to create.
  /// \param size The size of the texture to create.
  /// \param data The memory from where to load the image.
  Texture(Format format, glm::uvec2 size, const void *data);

  /// @brief Creates a 1D texture from a specifed format, width and memory.
  /// \param format The format of the texture to create.
  /// \param width The width of the texture to create.
  /// \param data The memory from where to load the image.
  Texture(Format format, int width, const void *data);

  /// @brief Destructor of the texture.
  ~Texture();

  /// @brief Loads a texture from a file.
  /// \param path The path of the image file to load.
  void load(const std::filesystem::path &path);

  /// @brief Loads a texture from a stream.
  /// \param input The stream of the image to load.
  void loadFromStream(std::istream &input);

  /// @brief Loads a texture from memory.
  /// \param size The size of the texture to load.
  /// \param data The memory from where to load the image.
  void loadFromMemory(glm::uvec2 size, const void *data);

  /// @brief Sets whether or not the texture has to be smooth.
  /// \param smooth true to enable smoothing, false otherwise.
  void setSmooth(bool smooth = true);

  /// @brief Indicates whether or not the texture has to be smooth.
  /// \return true if smoothing is enabled, false otherwise.
  [[nodiscard]] bool isSmooth() const noexcept;

  /// @brief Gets the size of the texture in pixels.
  /// \return The size of the texture in pixels.
  [[nodiscard]] glm::uvec2 getSize() const noexcept { return m_size; }

  /// @brief Binds a texture for rendering
  /// This function is for internal use only.
  /// \param texture The pointer to the texture to bind or 'nullptr' to not use a texture.
  static void bind(const Texture *texture);

  /// @brief Gets the native handle of the texture.
  /// \return The native handle of the texture.
  [[nodiscard]] unsigned int getHandle() const { return m_img_tex; }

private:
  void updateFilters();

private:
  Type m_type{Type::Texture2D};
  Format m_format{Format::Rgba};
  glm::uvec2 m_size{0, 0};
  bool m_smooth{false};
  unsigned int m_img_tex{0};
};
}
