#include <ngf/System/VideoDisplay.h>
#include <SDL.h>

namespace ngf {
VideoDisplay::VideoDisplay(int id) : m_id(id) {}

std::string VideoDisplay::getName() const {
  return SDL_GetDisplayName(m_id);
}

ngf::irect VideoDisplay::getBounds() const {
  SDL_Rect rect;
  SDL_GetDisplayBounds(m_id, &rect);
  return ngf::irect::fromPositionSize({rect.x, rect.y}, {rect.w, rect.h});
}

int VideoDisplay::getId() const { return m_id; }

[[nodiscard]] DisplayModeIterator VideoDisplay::getDisplayModes() const{
  return DisplayModeIterator(m_id);
}

VideoDisplayIterator::VideoDisplayIterator() : m_display(0) {}

VideoDisplayIterator::VideoDisplayIterator(int id) : m_display(id) {
}

VideoDisplayIterator VideoDisplayIterator::begin() const {
  return VideoDisplayIterator(0);
}

VideoDisplayIterator VideoDisplayIterator::end() const {
  auto count = SDL_GetNumVideoDisplays();
  return VideoDisplayIterator(count);
}

VideoDisplayIterator VideoDisplayIterator::cbegin() const {
  return VideoDisplayIterator(0);
}

VideoDisplayIterator VideoDisplayIterator::cend() const {
  auto count = SDL_GetNumVideoDisplays();
  return VideoDisplayIterator(count);
}

const VideoDisplayIterator VideoDisplayIterator::operator++(int) {
  auto result = *this;
  ++(*this);
  return result;
}

VideoDisplayIterator &VideoDisplayIterator::operator++() {
  m_display = VideoDisplay(m_display.getId() + 1);
  return *this;
}

bool VideoDisplayIterator::operator==(const VideoDisplayIterator &rhs) const {
  return rhs.m_display.getId() == m_display.getId();
}

bool VideoDisplayIterator::operator!=(const VideoDisplayIterator &rhs) const {
  return !(*this == rhs);
}

VideoDisplayIterator::reference VideoDisplayIterator::operator*() {
  return m_display;
}

VideoDisplayIterator::pointer VideoDisplayIterator::operator->() {
  return &m_display;
}
}