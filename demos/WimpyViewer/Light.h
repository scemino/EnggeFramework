#pragma once
#include <ngf/Graphics/Color.h>
#include <ngf/Graphics/Colors.h>
#include <glm/vec2.hpp>

struct Light {
  ngf::Color color{ngf::Colors::White};
  glm::ivec2 pos{0, 0};
  float brightness{1.0f}; ///< light brightness 1.0f...100.f
  float coneDirection{0}; ///< cone direction 0...360.f
  float coneAngle;     ///< cone angle 0...360.f
  float coneFalloff;   ///< cone falloff 0.f...1.0f
  float cutOffRadius;  ///< cutoff raduis
  float halfRadius;    ///< cone half radius 0.0f...1.0f
};
