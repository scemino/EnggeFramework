#pragma once
#include <ngf/System/TimeSpan.h>

namespace ngf {
class StopWatch {
public:
  StopWatch();

  [[nodiscard]] TimeSpan getElapsedTime() const;
  TimeSpan restart();

private:
  TimeSpan m_startTime{};
};
}
