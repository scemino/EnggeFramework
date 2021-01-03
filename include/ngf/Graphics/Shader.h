#pragma once
#include <cassert>
#include <map>
#include <string_view>
#include <vector>
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/Texture.h>

namespace ngf {
/// @brief An OpenGL vertex and/or fragment shader.
class Shader {
public:
  enum Type {
    Vertex,   ///< Type for a vertex shader.
    Fragment, ///< Type for a fragment (or pixel) shader.
  };

  /// @brief Creates an empty shader, invalid until you load a shader.
  Shader() = default;
  /// @brief Loads the vertex and fragment shader from memory.
  Shader(const char *vertexShader, const char *fragmentShader);
  /// @brief Destructor of the shader.
  ~Shader();

  /// @brief This forbids to copy a shader.
  Shader(const Shader&) = delete;
  /// @brief This forbids to assign a shader.
  Shader& operator=(const Shader&) = delete;

  /// @brief Loads the vertex and fragment shader from memory.
  void load(const char *vertexShader, const char *fragmentShader);

public:
  /// @brief Sets an attribute of a shader with the specified vector value.
  /// \param name The name of the attribute variable in GLSL.
  /// \param value The integer value to set to the variable.
  void setAttribute(std::string_view name, const glm::vec2 &value) const;
  /// @brief Sets a uniform shader parameter with the specified integer value.
  /// \param name The name of the uniform variable in GLSL.
  /// \param value The integer value to set to the variable.
  void setUniform(std::string_view name, int value) const;
  /// @brief Sets a uniform shader parameter with the specified floating point value.
  /// \param name The name of the uniform variable in GLSL.
  /// \param value The floating point value to set to the variable.
  void setUniform(std::string_view name, float value) const;
  void setUniformArray(std::string_view name, const float *vectorArray, std::size_t length) const;
  /// @brief Sets a uniform shader parameter with the specified vector of 2 floating point value.
  /// \param name The name of the uniform variable in GLSL.
  /// \param value The vector of 2 floating point value to set to the variable.
  void setUniform(std::string_view name, glm::vec2 value) const;
  void setUniformArray(std::string_view name, const glm::vec2 *vectorArray, std::size_t length) const;
  /// @brief Sets a uniform shader parameter with the specified vector of 3 floating point value.
  /// \param name The name of the uniform variable in GLSL.
  /// \param value The vector of 3 floating point value to set to the variable.
  void setUniform(std::string_view name, glm::vec3 value) const;
  void setUniformArray(std::string_view name, const glm::vec3* vectorArray, std::size_t length) const;
  void setUniformArray3(std::string_view name, const ngf::Color* vectorArray, std::size_t length) const;
  /// @brief Sets a uniform shader parameter with the specified vector of 4 floating point value.
  /// \param name The name of the uniform variable in GLSL.
  /// \param value The vector of 4 floating point value to set to the variable.
  void setUniform(std::string_view name, glm::vec4 value) const;
  /// @brief Sets a uniform shader parameter with the specified matrix 3x3 value.
  /// \param name The name of the uniform variable in GLSL.
  /// \param value The matrix 3x3 value to set to the variable.
  void setUniform(std::string_view name, const glm::mat3 &value) const;
  /// @brief Sets a uniform shader parameter with the specified matrix 4x4 value.
  /// \param name The name of the uniform variable in GLSL.
  /// \param value The 4x4 matrix value to set to the variable.
  void setUniform(std::string_view name, const glm::mat4 &value) const;
  /// @brief Sets a uniform shader parameter with the specified color of 3 floating point value.
  /// \param name The name of the uniform variable in GLSL.
  /// \param value The color of 3 floating point value to set to the variable.
  void setUniform3(std::string_view name, const Color& value) const;
  /// @brief Sets a uniform shader parameter with the specified color of 4 floating point value.
  /// \param name The name of the uniform variable in GLSL.
  /// \param value The color of 4 floating point value to set to the variable.
  void setUniform4(std::string_view name, const Color& value) const;
  /// @brief Sets a uniform shader parameter with the specified texture.
  /// \param name The name of the uniform variable in GLSL.
  /// \param texture The texture to set to the variable.
  void setUniform(std::string_view name, const Texture &texture);
  /// @brief Binds a shader for rendering.
  /// This function is for internal use only.
  /// \param shader The shader to bind, or null to not use a shader.
  static void bind(const Shader *shader);

  [[nodiscard]] int getAttributeLocation(std::string_view name) const;

private:
  [[nodiscard]] int getUniformLocation(std::string_view name) const;

private:
  struct Guard;

private:
  unsigned m_program{0};
  std::map<int, const Texture *> m_textures{};
};
}
