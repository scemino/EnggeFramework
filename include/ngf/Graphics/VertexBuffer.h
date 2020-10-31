#pragma once
#include <array>
#include <GL/glew.h>

namespace ngf {
/// @brief Data in the graphics memory.
class VertexBuffer {
public:
  enum class Type {
    Array,
    Element
  };

  /// @brief Creates a new data store for a buffer object.
  VertexBuffer() noexcept;
  /// @brief Forbids to copy a VertexBuffer.
  VertexBuffer(const VertexBuffer&) = delete;
  /// @brief Forbids to assign a VertexBuffer.
  VertexBuffer& operator=(const VertexBuffer&) = delete;
  /// @brief Destructor of a VertexBuffer.
  ~VertexBuffer() noexcept;

  /// Sets new data to a buffer object.
  /// \param type: Specifies the target to which the buffer object is bound.
  /// \param size: Specifies the size in bytes of the buffer object's new data store.
  /// \param data: Specifies a pointer to data that will be copied into the data store for initialization, or nullptr if no data is to be copied.
  void buffer(Type type, size_t size, const void *data);

  /// @brief Binds a vertex buffer.
  /// \param vertexBuffer A pointer to a buffer or `nullptr`.
  static void bind(const VertexBuffer* vertexBuffer);

private:
  std::array<unsigned int,2> m_buffers{};
};
}
