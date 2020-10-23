#pragma once
#include <cassert>
#include <map>
#include <string_view>
#include <vector>
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Texture.h"

namespace ngf {
class Shader {
public:
  enum Type {
    Vertex,
    Fragment,
  };

  Shader() = default;
  Shader(const char *vertexShader, const char *fragmentShader);
  ~Shader();

  void load(const char *vertexShader, const char *fragmentShader);

  [[nodiscard]] unsigned int getHandle() const {
    return m_program;
  }

public:
  void setUniform(std::string_view name, int value) const;
  void setAttribute(std::string_view name, const glm::vec2 &value) const;
  void setUniform(std::string_view name, const glm::mat3 &value) const;
  void setUniform(std::string_view name, const glm::mat4 &value) const;
  void setUniform(std::string_view name, const Texture &tex);
  static void bind(const Shader *shader);
  [[nodiscard]] int getAttributeLocation(std::string_view name) const;
  [[nodiscard]] int getUniformLocation(std::string_view name) const;

private:
  struct Guard;

private:
  unsigned m_program{0};
  std::map<int, const Texture *> m_textures{};
};
}
