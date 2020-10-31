#pragma once
#include <ngf/System/TimeSpan.h>

namespace ngf {
/// @brief Provides a set of methods and properties that you can use to accurately measure elapsed time.
class StopWatch {
public:
  /// @brief Creates a Stopwatch.
  StopWatch();

  /// @brief Gets the total elapsed time measured.
  /// \return The total elapsed time measured.
  [[nodiscard]] TimeSpan getElapsedTime() const;
  /// @brief Stops time interval measurement, resets the elapsed time to zero, and starts measuring elapsed time.
  /// \return The total elapsed time measured.
  TimeSpan restart();

private:
  TimeSpan m_startTime{};
};
}
