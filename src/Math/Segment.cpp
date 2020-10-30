#include <cmath>
#include <ngf/Math/Segment.h>

namespace ngf {

namespace {
inline constexpr float Epsilon = 1E-9;
}

Segment::Segment(const glm::vec2 &start, const glm::vec2 &end) {
  this->start = glm::vec2(start);
  this->end = glm::vec2(end);
  this->left = fmin(start.x, end.x);
  this->right = fmax(start.x, end.x);
  this->top = fmin(start.y, end.y);
  this->bottom = fmax(start.y, end.y);
  a = start.y - end.y;
  b = end.x - start.x;
  c = -a * start.x - b * start.y;
  normalize();
}

void Segment::normalize() {
  float z = sqrtf(a * a + b * b);
  if (fabs(z) > Epsilon)
    a /= z, b /= z, c /= z;
}

float Segment::distance(glm::vec2 p) const { return a * p.x + b * p.y + c; }

bool operator==(Segment lhs, const Segment &rhs) {
  return lhs.start == rhs.start && lhs.end == rhs.end;
}

bool operator!=(Segment lhs, const Segment &rhs) {
  return lhs.start != rhs.start || lhs.end != rhs.end;
}
}