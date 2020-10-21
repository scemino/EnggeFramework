#include <ngf/System/StopWatch.h>
#include <SDL.h>

namespace ngf{
StopWatch::StopWatch() : m_startTime(TimeSpan::milliseconds(SDL_GetTicks())) {
}

TimeSpan StopWatch::getElapsedTime() const {
  return TimeSpan::milliseconds(SDL_GetTicks()) - m_startTime;
}

TimeSpan StopWatch::restart() {
  TimeSpan now = TimeSpan::milliseconds(SDL_GetTicks());
  TimeSpan elapsed = now - m_startTime;
  m_startTime = now;
  return elapsed;
}
}
