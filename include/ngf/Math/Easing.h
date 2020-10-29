#pragma once
#include <cassert>
#include <cmath>
#include <glm/gtx/easing.hpp>

namespace ngf::Easing {
static constexpr float linear(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);
  return t;
}

static constexpr float quadraticEaseIn(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);
  return t * t;
}

static constexpr float quadraticEaseOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);
  return -(t * (t - 2.f));
}

static constexpr float quadraticEaseInOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);
  if (t < 0.5f) {
    return 2.f * t * t;
  } else {
    return (-2.f * t * t) + (4.f * t) - 1.f;
  }
}

static constexpr float cubicEaseIn(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);
  return t * t * t;
}

static constexpr float cubicEaseOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);
  const float f = t - 1.f;
  return f * f * f + 1.f;
}

static constexpr float cubicEaseInOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  if (t < 0.5f) {
    return 4.f * t * t * t;
  }
  const float f = ((2.f * t) - 2.f);
  return 0.5f * f * f * f + 1.f;
}

static constexpr float quarticEaseIn(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);
  return t * t * t * t;
}

static constexpr float quarticEaseOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);
  const float f = (t - 1.f);
  return f * f * f * (1.f - t) + 1.f;
}

static constexpr float quarticEaseInOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);
  if (t < 0.5f) {
    return 8.f * t * t * t * t;
  }
  const float f = (t - 1.f);
  return -8.f * f * f * f * f + 1.f;
}

static float sineEaseIn(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  return sin((t - 1.f) * glm::half_pi<float>()) + 1.f;
}

static float sineEaseOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);
  return sin(t * glm::half_pi<float>());
}

static float sineEaseInOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  return 0.5f * (1.f - cos(t * glm::pi<float>()));
}

static float circularEaseIn(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  return 1.f - sqrt(1.f - (t * t));
}

static float circularEaseOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  return sqrt((2.f - t) * t);
}

static constexpr float circularEaseInOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  if (t < 0.5f) {
    return 0.5f * (1.f - std::sqrt(1.f - 4.f * (t * t)));
  } else {
    return 0.5f * (std::sqrt(-((2.f * t) - 3.f) * ((2.f * t) - 1.f)) + 1.f);
  }
}

static constexpr float exponentialEaseIn(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  if (t <= 0.f)
    return t;
  const float Complementary = t - 1.f;
  return glm::pow(2.f, Complementary * 10.f);
}

static constexpr float exponentialEaseOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  if (t >= 1.f)
    return t;
  return 1.f - glm::pow(2.f, -10.f * t);
}

static constexpr float exponentialEaseInOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  if (t < 0.5f)
    return 0.5f * glm::pow(2.f, (20.f * t) - 10.f);
  return -0.5f * glm::pow(2.f, (-20.f * t) + 10.f) + 1.f;
}

static float elasticEaseIn(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  return std::sin(13.f * glm::half_pi<float>() * t)
      * glm::pow(2.f, 10.f * (t - 1.f));
}

static float elasticEaseOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  return std::sin(-13.f * glm::half_pi<float>() * (t + 1.f))
      * glm::pow(2.f, -10.f * t) + 1.f;
}

static constexpr float elasticEaseInOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  if (t < 0.5f)
    return 0.5f * std::sin(13.f * glm::half_pi<float>() * (2.f * t)) * glm::pow(2.f, 10.f * ((2.f * t) - 1.f));
  return 0.5f * (std::sin(-13.f * glm::half_pi<float>() * ((2.f * t - 1.f) + 1.f))
      * glm::pow(2.f, -10.f * (2.f * t - 1.f)) + 2.f);
}

static constexpr float backEaseIn(float t, float o) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  float z = ((o + 1.f) * t) - o;
  return (t * t * z);
}

static constexpr float backEaseOut(float t, float o) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  float n = t - 1.f;
  float z = ((o + 1.f) * n) + o;
  return (n * n * z) + 1.f;
}

static constexpr float backEaseInOut(float t, float o) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  float s = o * 1.525f;
  float x = 0.5f;
  float n = t / 0.5f;

  if (n < 1.f) {
    float z = ((s + 1.f) * n) - s;
    float m = n * n * z;
    return x * m;
  }
  n -= 2.f;
  float z = ((s + 1.f) * n) + s;
  float m = (n * n * z) + 2.f;
  return x * m;
}

static constexpr float backEaseIn(float t) {
  return backEaseIn(t, 1.70158f);
}

static constexpr float backEaseOut(float t) {
  return backEaseOut(t, 1.70158f);
}

static constexpr float backEaseInOut(float t) {
  return backEaseInOut(t, 1.70158f);
}

static constexpr float bounceEaseOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  if (t < 4.0f / 11.0f) {
    return (121.f * t * t) / 16.f;
  }
  if (t < 8.0f / 11.0f) {
    return (363.0f / 40.0f * t * t) - (99.0f / 10.0f * t)
        + 17.0f / 5.0f;
  }
  if (t < 9.0f / 10.0f) {
    return (4356.0f / 361.0f * t * t) - (35442.0f / 1805.0f * t)
        + 16061.0f / 1805.0f;
  }
  return (54.0f / 5.0f * t * t) - (513.0f / 25.0f * t)
      + 268.0f / 25.0f;
}

static constexpr float bounceEaseIn(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  return 1.f - bounceEaseOut(1.f - t);
}

template<typename genType>
static constexpr float bounceEaseInOut(float t) {
  assert(t >= 0.f);
  assert(t <= 1.f);

  if (t < static_cast<genType>(0.5)) {
    return 0.5f * (1.f - bounceEaseOut(t * static_cast<genType>(2)));
  } else {
    return 0.5f * bounceEaseOut(t * static_cast<genType>(2) - 1.f) + 0.5f;
  }
}
}