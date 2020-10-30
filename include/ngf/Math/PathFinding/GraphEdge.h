#pragma once
#include <vector>
#include <memory>

namespace ngf {
struct GraphEdge {
  int from{0};
  int to{0};
  float cost{0.f};

  GraphEdge(int from, int to, float cost);
};

std::ostream &operator<<(std::ostream &os, const GraphEdge &edge);
}
