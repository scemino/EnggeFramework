#pragma once
#include <glm/vec2.hpp>

class Camera final {
public:
  Camera() = default;
  Camera(glm::vec2 p, glm::vec2 s) : position(p), size(s) {
  }

public:
  glm::vec2 position;
  glm::vec2 size;
  float zoom{1.f};
};
