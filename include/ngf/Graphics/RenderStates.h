#pragma once
#include <glm/mat3x3.hpp>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/Texture.h>
#include <ngf/Graphics/BlendMode.h>

namespace ngf {
/// @brief Define the states used for drawing to a RenderTarget.
struct RenderStates {
  glm::mat3 transform{1.0f};       ///< The transformation matrix.
  const Texture *texture{nullptr}; ///< The texture.
  Shader *shader{nullptr};         ///< The shader.
  BlendMode mode{BlendAlpha};      ///< The blending mode.
};
}

