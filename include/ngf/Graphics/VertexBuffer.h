#pragma once
#include <array>
#include <GL/glew.h>

namespace ngf {
class VertexBuffer {
public:
  enum class Type {
    Array,
    Element
  };

  /// Creates a new data store for a buffer object.
  VertexBuffer() noexcept;
  ~VertexBuffer() noexcept;
  VertexBuffer(const VertexBuffer&) = delete;
  VertexBuffer& operator=(const VertexBuffer&) = delete;

  /// Sets new data to a buffer object.
  /// \param type: Specifies the target to which the buffer object is bound.
  /// \param size: Specifies the size in bytes of the buffer object's new data store.
  /// \param data: Specifies a pointer to data that will be copied into the data store for initialization, or nullptr if no data is to be copied.
  void buffer(Type type, size_t size, const void *data);

  static void bind(const VertexBuffer* pVertexBuffer);

private:
  std::array<unsigned int,2> m_buffers{};
};
}
