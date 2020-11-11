#pragma once
#include <algorithm>
#include <glm/vec2.hpp>

namespace ngf {
template<int N, typename T, glm::qualifier Q>
glm::vec<N, T, Q> min(glm::vec<N, T, Q> lhs, glm::vec<N, T, Q> rhs) {
  glm::vec<N, T, Q> out = {};
  for (std::size_t i = 0; i < N; ++i) {
    out[i] = std::min(lhs[i], rhs[i]);
  }
  return out;
}

template<int N, typename T, glm::qualifier Q>
glm::vec<N, T, Q> max(glm::vec<N, T, Q> lhs, glm::vec<N, T, Q> rhs) {
  glm::vec<N, T, Q> out = {};
  for (std::size_t i = 0; i < N; ++i) {
    out[i] = std::max(lhs[i], rhs[i]);
  }
  return out;
}
}