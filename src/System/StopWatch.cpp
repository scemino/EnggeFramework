#include <ngf/System/StopWatch.h>
#include <SDL2/SDL.h>
#include "SdlSystem.h"

namespace ngf{
StopWatch::StopWatch() : m_startTime(TimeSpan::milliseconds(SDL_CHECK_EXPR(SDL_GetTicks()))) {
}

TimeSpan StopWatch::getElapsedTime() const {
  return TimeSpan::milliseconds(SDL_CHECK_EXPR(SDL_GetTicks())) - m_startTime;
}

TimeSpan StopWatch::restart() {
  TimeSpan now = TimeSpan::milliseconds(SDL_CHECK_EXPR(SDL_GetTicks()));
  TimeSpan elapsed = now - m_startTime;
  m_startTime = now;
  return elapsed;
}
}
