#pragma once
#include <cstddef>

namespace ngf {
class Times {
public:
  constexpr Times() noexcept = default;
  constexpr Times(int number) noexcept: m_value(number) {}

  inline Times &operator--() {
    if (m_isInfinity)
      return *this;
    m_value--;
    return *this;
  }

  inline Times operator--(int) {
    Times ret = *this;
    this->operator--();
    return ret;
  }

  inline Times &operator++() {
    if (m_isInfinity)
      return *this;
    m_value++;
    return *this;
  }

  inline Times operator++(int) {
    Times ret = *this;
    this->operator++();
    return ret;
  }

private:
  Times(std::nullptr_t) : m_isInfinity(true) {}

public:
  static Times Infinity;
  bool m_isInfinity{false};
  int m_value{0};
};

inline bool operator==(const Times &lhs, const Times &rhs) {
  if (lhs.m_isInfinity && rhs.m_isInfinity)
    return true;
  if (lhs.m_isInfinity != rhs.m_isInfinity)
    return false;
  return lhs.m_value == rhs.m_value;
}

inline bool operator!=(const Times &lhs, const Times &rhs) {
  return !(lhs == rhs);
}

inline bool operator<=(const Times &lhs, const Times &rhs) {
  if (lhs.m_isInfinity && rhs.m_isInfinity)
    return true;
  if (!lhs.m_isInfinity && rhs.m_isInfinity)
    return true;
  if (lhs.m_isInfinity && !rhs.m_isInfinity)
    return false;
  return lhs.m_value <= rhs.m_value;
}

inline bool operator>(const Times &lhs, const Times &rhs) {
  return !(lhs <= rhs);
}

inline bool operator>=(const Times &lhs, const Times &rhs) {
  return (lhs == rhs) || (lhs > rhs);
}

inline bool operator<(const Times &lhs, const Times &rhs) {
  if (lhs.m_isInfinity && rhs.m_isInfinity)
    return false;
  if (!lhs.m_isInfinity && rhs.m_isInfinity)
    return true;
  if (lhs.m_isInfinity && !rhs.m_isInfinity)
    return false;
  return lhs.m_value < rhs.m_value;
}
}