#pragma once
#include <cstddef>

namespace ngf {
/// @brief Represents a number of times.
/// @sa Tween.
class Times {
public:
  /// @brief Creates an empty number of time.
  constexpr Times() noexcept = default;
  /// @brief Creates a number of times from a specified number.
  /// \param number Bumber of times.
  constexpr Times(int number) noexcept: m_value(number) {}

  /// @brief Decrements a number of times.
  /// \return The new number of times.
  inline Times &operator--() {
    if (m_isInfinity)
      return *this;
    m_value--;
    return *this;
  }

  /// @brief Decrements a number of times.
  /// \return The new number of times.
  inline Times operator--(int) {
    Times ret = *this;
    this->operator--();
    return ret;
  }

  /// @brief Increments a number of times.
  /// \return The new number of times.
  inline Times &operator++() {
    if (m_isInfinity)
      return *this;
    m_value++;
    return *this;
  }

  /// @brief Increments a number of times.
  /// \return The new number of times.
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