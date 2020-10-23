#pragma once
#include <glm/mat3x3.hpp>
#include <ngf/Graphics/Shader.h>
#include <ngf/Graphics/Texture.h>

namespace ngf {
struct RenderStates {
  glm::mat3 transform{1.0f};
  const Texture *texture{nullptr};
  Shader *shader{nullptr};
};
}

