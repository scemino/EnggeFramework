#include <ngf/System/DisplayMode.h>
#include <SDL2/SDL.h>
#include "SdlSystem.h"

namespace ngf {
namespace {
DisplayModeFormat toDisplayModeFormat(Uint32 format) {
  switch (format) {
  case SDL_PIXELFORMAT_UNKNOWN: return DisplayModeFormat::Unknown;
  case SDL_PIXELFORMAT_INDEX1LSB: return DisplayModeFormat::INDEX1LSB;
  case SDL_PIXELFORMAT_INDEX1MSB: return DisplayModeFormat::INDEX1MSB;
  case SDL_PIXELFORMAT_INDEX4LSB: return DisplayModeFormat::INDEX4LSB;
  case SDL_PIXELFORMAT_INDEX4MSB: return DisplayModeFormat::INDEX4MSB;
  case SDL_PIXELFORMAT_INDEX8: return DisplayModeFormat::INDEX8;
  case SDL_PIXELFORMAT_RGB332: return DisplayModeFormat::RGB332;
  case SDL_PIXELFORMAT_RGB444: return DisplayModeFormat::RGB444;
  case SDL_PIXELFORMAT_RGB555: return DisplayModeFormat::RGB555;
  case SDL_PIXELFORMAT_BGR555: return DisplayModeFormat::BGR555;
  case SDL_PIXELFORMAT_ARGB4444: return DisplayModeFormat::ARGB4444;
  case SDL_PIXELFORMAT_RGBA4444: return DisplayModeFormat::RGBA4444;
  case SDL_PIXELFORMAT_ABGR4444: return DisplayModeFormat::ABGR4444;
  case SDL_PIXELFORMAT_BGRA4444: return DisplayModeFormat::BGRA4444;
  case SDL_PIXELFORMAT_ARGB1555: return DisplayModeFormat::ARGB1555;
  case SDL_PIXELFORMAT_RGBA5551: return DisplayModeFormat::RGBA5551;
  case SDL_PIXELFORMAT_ABGR1555: return DisplayModeFormat::ABGR1555;
  case SDL_PIXELFORMAT_BGRA5551: return DisplayModeFormat::BGRA5551;
  case SDL_PIXELFORMAT_RGB565: return DisplayModeFormat::RGB565;
  case SDL_PIXELFORMAT_BGR565: return DisplayModeFormat::BGR565;
  case SDL_PIXELFORMAT_RGB24: return DisplayModeFormat::RGB24;
  case SDL_PIXELFORMAT_BGR24: return DisplayModeFormat::BGR24;
  case SDL_PIXELFORMAT_RGB888: return DisplayModeFormat::RGB888;
  case SDL_PIXELFORMAT_RGBX8888: return DisplayModeFormat::RGBX8888;
  case SDL_PIXELFORMAT_BGR888: return DisplayModeFormat::BGR888;
  case SDL_PIXELFORMAT_BGRX8888: return DisplayModeFormat::BGRX8888;
  case SDL_PIXELFORMAT_ARGB8888: return DisplayModeFormat::ARGB8888;
  case SDL_PIXELFORMAT_RGBA8888: return DisplayModeFormat::RGBA8888;
  case SDL_PIXELFORMAT_ABGR8888: return DisplayModeFormat::ABGR8888;
  case SDL_PIXELFORMAT_BGRA8888: return DisplayModeFormat::BGRA8888;
  case SDL_PIXELFORMAT_ARGB2101010: return DisplayModeFormat::ARGB2101010;
  case SDL_PIXELFORMAT_YV12: return DisplayModeFormat::YV12;
  case SDL_PIXELFORMAT_IYUV: return DisplayModeFormat::IYUV;
  case SDL_PIXELFORMAT_YUY2: return DisplayModeFormat::YUY2;
  case SDL_PIXELFORMAT_UYVY: return DisplayModeFormat::UYVY;
  case SDL_PIXELFORMAT_YVYU: return DisplayModeFormat::YVYU;
  case SDL_PIXELFORMAT_NV12: return DisplayModeFormat::NV12;
  case SDL_PIXELFORMAT_NV21: return DisplayModeFormat::NV21;
  default: return DisplayModeFormat::Unknown;
  }
}
}

DisplayMode::DisplayMode(int displayId, int modeId)
    : m_displayId(displayId), m_modeId(modeId) {
  SDL_DisplayMode mode;
  SDL_CHECK(SDL_GetDisplayMode(displayId, modeId, &mode));
  size = {mode.w, mode.h};
  bitsPerPixel = SDL_BITSPERPIXEL(mode.format);
  refreshRate = mode.refresh_rate;
  format = toDisplayModeFormat(mode.format);
}

DisplayModeIterator::DisplayModeIterator(int displayId) : m_mode(displayId, 0) {}

DisplayModeIterator::DisplayModeIterator(int displayId, int modeId) : m_mode(displayId, modeId) {
}

DisplayModeIterator DisplayModeIterator::begin() const {
  return DisplayModeIterator(m_mode.m_displayId, 0);
}

DisplayModeIterator DisplayModeIterator::end() const {
  auto count = SDL_GetNumDisplayModes(m_mode.m_displayId);
  return DisplayModeIterator(m_mode.m_displayId, count);
}

DisplayModeIterator DisplayModeIterator::cbegin() const {
  return DisplayModeIterator(m_mode.m_displayId, 0);
}

DisplayModeIterator DisplayModeIterator::cend() const {
  auto count = SDL_GetNumDisplayModes(m_mode.m_displayId);
  return DisplayModeIterator(m_mode.m_displayId, count);
}

const DisplayModeIterator DisplayModeIterator::operator++(int) {
  auto result = *this;
  ++(*this);
  return result;
}

DisplayModeIterator &DisplayModeIterator::operator++() {
  m_mode = DisplayMode(m_mode.m_displayId, m_mode.m_modeId + 1);
  return *this;
}

bool DisplayModeIterator::operator==(const DisplayModeIterator &rhs) const {
  return rhs.m_mode.m_displayId == m_mode.m_displayId &&
      rhs.m_mode.m_modeId == m_mode.m_modeId;
}

bool DisplayModeIterator::operator!=(const DisplayModeIterator &rhs) const {
  return !(*this == rhs);
}

DisplayModeIterator::reference DisplayModeIterator::operator*() {
  return m_mode;
}

DisplayModeIterator::pointer DisplayModeIterator::operator->() {
  return &m_mode;
}
}
