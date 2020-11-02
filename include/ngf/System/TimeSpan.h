#pragma once
#include <cmath>
#include <limits>
#include <stdexcept>

namespace ngf {
/// @brief Represents a time interval.
struct TimeSpan {
private:
  static const long TicksPerMillisecond = 10000;
  static constexpr float MillisecondsPerTick = 1.0 / TicksPerMillisecond;

  static const long TicksPerSecond = TicksPerMillisecond * 1000;
  static constexpr float SecondsPerTick = 1.0 / TicksPerSecond;

  static const long TicksPerMinute = TicksPerSecond * 60;
  static constexpr float MinutesPerTick = 1.0 / TicksPerMinute;

  static const long MaxSeconds =
      std::numeric_limits<long>::max() / TicksPerSecond;
  static const long MinSeconds =
      std::numeric_limits<long>::min() / TicksPerSecond;

  static const long MaxMilliSeconds =
      std::numeric_limits<long>::max() / TicksPerMillisecond;
  static const long MinMilliSeconds =
      std::numeric_limits<long>::min() / TicksPerMillisecond;

  static const int MillisPerSecond = 1000;
  static const int MillisPerMinute = MillisPerSecond * 60;

public:
  /// @brief Creates a zero time interval.
  TimeSpan() noexcept = default;
  /// @brief Creates a TimeSpan with the specified number of ticks.
  /// \param ticks A time period expressed in 100-nanosecond units.
  explicit TimeSpan(long ticks) noexcept: m_ticks(ticks) {
  }

  /// @brief Creates a TimeSpan from a specified number of hours, minutes and seconds.
  /// \param hour Number of hours.
  /// \param minute Number of minutes.
  /// \param second Number of seconds.
  TimeSpan(int hour, int minute, int second)
      : m_ticks(toTicks(hour, minute, second)) {
  }

  /// Creates a TimeSpan that represents a specified number of seconds.
  /// \param value The number of seconds
  /// \return The TimeSpan that represents a specified number of seconds.
  static TimeSpan seconds(float value) {
    return interval(value, MillisPerSecond);
  }

  /// Creates a TimeSpan that represents a specified number of milliseconds.
  /// \param value The number of milliseconds
  /// \return The TimeSpan that represents a specified number of milliseconds.
  static TimeSpan milliseconds(float value) {
    return interval(value, 1);
  }

  /// Creates a TimeSpan that represents a specified number of minutes.
  /// \param value The number of minutes
  /// \return The TimeSpan that represents a specified number of minutes.
  static TimeSpan minutes(float value) {
    return interval(value, MillisPerMinute);
  }

  /// @brief Gets the number of ticks that represent the value of the current TimeSpan.
  /// \return The number of ticks that represent the value of the current TimeSpan.
  [[nodiscard]] long getTicks() const {
    return m_ticks;
  }

  /// @brief Gets the milliseconds component of the time interval represented by the current TimeSpan.
  /// \return The milliseconds component of the time interval represented by the current TimeSpan.
  [[nodiscard]] int getMilliseconds() const {
    return (int) ((m_ticks / TicksPerMillisecond) % 1000);
  }

  /// @brief Gets the minutes component of the time interval represented by the current TimeSpan.
  /// \return The minutes component of the time interval represented by the current TimeSpan.
  [[nodiscard]] int getMinutes() const {
    return (int) ((m_ticks / TicksPerMinute) % 60);
  }

  /// @brief Gets the seconds component of the time interval represented by the current TimeSpan.
  /// \return The minutes seconds of the time interval represented by the current TimeSpan.
  [[nodiscard]] int getSeconds() const {
    return (int) ((m_ticks / TicksPerSecond) % 60);
  }

  /// @brief Gets the value of the current TimeSpan expressed in whole and fractional milliseconds.
  /// \return The value of the current TimeSpan expressed in whole and fractional milliseconds.
  [[nodiscard]] float getTotalMilliseconds() const {
    float temp = (float) m_ticks * MillisecondsPerTick;
    if (temp > MaxMilliSeconds)
      return (float) MaxMilliSeconds;

    if (temp < MinMilliSeconds)
      return (float) MinMilliSeconds;

    return temp;
  }

  /// @brief Gets the value of the current TimeSpan expressed in whole and fractional minutes.
  /// \return The value of the current TimeSpan expressed in whole and fractional minutes.
  [[nodiscard]] float getTotalMinutes() const {
    return (float) m_ticks * MinutesPerTick;
  }

  /// @brief Gets the value of the current TimeSpan expressed in whole and fractional seconds.
  /// \return The value of the current TimeSpan expressed in whole and fractional seconds.
  [[nodiscard]] float getTotalSeconds() const {
    return (float) m_ticks * SecondsPerTick;
  }

private:
  static long toTicks(int hour, int minute, int second) {
    long totalSeconds =
        (long) hour * 3600 + (long) minute * 60 + (long) second;
    if (totalSeconds > MaxSeconds || totalSeconds < MinSeconds)
      throw std::invalid_argument("totalSeconds is out of the bounds");
    return totalSeconds * TicksPerSecond;
  }

private:
  static TimeSpan interval(float value, int scale) {
    if (std::isnan(value))
      throw std::invalid_argument("value cannot be nan (not a number)");
    float tmp = value * scale;
    float millis = tmp + (value >= 0 ? 0.5 : -0.5);
    if ((millis > std::numeric_limits<long>::max() / TicksPerMillisecond) ||
        (millis < std::numeric_limits<long>::min() / TicksPerMillisecond))
      throw std::overflow_error("Specifed time is tool long");
    return TimeSpan{(long) millis * TicksPerMillisecond};
  }

public:
  static const TimeSpan Zero; ///< Represents the zero TimeSpan value.

private:
  long m_ticks{0}; ///< The number of ticks that represent the value of the current TimeSpan.
};

/// @brief Compares 2 TimeSpan for equality.
/// \param left Left TimeSpan operand.
/// \param right Right TimeSpan operand.
/// \return true if they are equals, false otherwise.
bool operator==(const TimeSpan &left, const TimeSpan &right);
/// @brief Compares 2 TimeSpan for inequality.
/// \param left Left TimeSpan operand.
/// \param right Right TimeSpan operand.
/// \return true if they are not equals, false otherwise.
bool operator!=(const TimeSpan &left, const TimeSpan &right);
/// @brief Indicates whether a specified TimeSpan is less than another specified TimeSpan.
/// \param left Left TimeSpan operand.
/// \param right Right TimeSpan operand.
/// \return true if the value of left is less than the value of right; otherwise, false.
bool operator<(const TimeSpan &left, const TimeSpan &right);
/// @brief Indicates whether a specified TimeSpan is greater than another specified TimeSpan.
/// \param left Left TimeSpan operand.
/// \param right Right TimeSpan operand.
/// \return true if the value of left is greater than the value of right; otherwise, false.
bool operator>(const TimeSpan &left, const TimeSpan &right);
/// @brief Indicates whether a specified TimeSpan is less than or equal to another specified TimeSpan.
/// \param left Left TimeSpan operand.
/// \param right Right TimeSpan operand.
/// \return true if the value of left is less than or equal to the value of right; otherwise, false.
bool operator<=(const TimeSpan &left, const TimeSpan &right);
/// @brief Indicates whether a specified TimeSpan is greater than or equal to another specified TimeSpan.
/// \param left Left TimeSpan operand.
/// \param right Right TimeSpan operand.
/// \return true if the value of left is greater than or equal to the value of right; otherwise, false.
bool operator>=(const TimeSpan &left, const TimeSpan &right);
/// @brief Adds two specified TimeSpan.
/// \param left Left TimeSpan operand.
/// \param right Right TimeSpan operand.
/// \return A TimeSpan whose value is the sum of the values of left and right.
TimeSpan operator+(const TimeSpan &left, const TimeSpan &right);
/// @brief Adds two specified TimeSpan and assigns the result to left TimeSpan operand.
/// \param left Left TimeSpan operand.
/// \param right Right TimeSpan operand.
/// \return A TimeSpan whose value is the sum of the values of left and right.
TimeSpan &operator+=(TimeSpan &left, const TimeSpan &right);
/// @brief Subtracts a specified TimeSpan from another specified TimeSpan.
/// \param left Left TimeSpan operand.
/// \param right Right TimeSpan operand.
/// \return A TimeSpan whose value is the result of the value of left minus the value of right.
TimeSpan operator-(const TimeSpan &left, const TimeSpan &right);
/// @brief Subtracts a specified TimeSpan from another specified TimeSpan and assigns the result to left TimeSpan operand.
/// \param left Left TimeSpan operand.
/// \param right Right TimeSpan operand.
/// \return A TimeSpan whose value is the result of the value of left minus the value of right.
TimeSpan &operator-=(TimeSpan &left, const TimeSpan &right);

}

