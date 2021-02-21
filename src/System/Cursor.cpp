#include <ngf/System/Cursor.h>
#include <SDL2/SDL.h>
#include <stdexcept>
#include "SdlSystem.h"

namespace ngf {
namespace {
SDL_SystemCursor getSDLSystemCursor(Cursor::Type type) {
  switch (type) {
  case Cursor::Arrow:return SDL_SYSTEM_CURSOR_ARROW;
  case Cursor::ArrowWait:return SDL_SYSTEM_CURSOR_WAITARROW;
  case Cursor::Wait:return SDL_SYSTEM_CURSOR_WAIT;
  case Cursor::Text:return SDL_SYSTEM_CURSOR_IBEAM;
  case Cursor::Hand:return SDL_SYSTEM_CURSOR_HAND;
  case Cursor::SizeHorizontal:return SDL_SYSTEM_CURSOR_SIZEWE;
  case Cursor::SizeVertical:return SDL_SYSTEM_CURSOR_SIZENS;
  case Cursor::SizeTopLeftBottomRight:return SDL_SYSTEM_CURSOR_SIZENWSE;
  case Cursor::SizeBottomLeftTopRight:return SDL_SYSTEM_CURSOR_SIZENESW;
  case Cursor::SizeAll:return SDL_SYSTEM_CURSOR_SIZEALL;
  case Cursor::Cross:return SDL_SYSTEM_CURSOR_CROSSHAIR;
  case Cursor::NotAllowed:return SDL_SYSTEM_CURSOR_NO;
  }
  return SDL_SYSTEM_CURSOR_ARROW;
}
}

Cursor::Cursor(SDL_Cursor *cursor) : m_cursor(cursor), m_freeCursor(false) {
}

Cursor::Cursor(Type type)
    : m_cursor(SDL_CHECK_EXPR(SDL_CreateSystemCursor(getSDLSystemCursor(type)))) {
  if (m_cursor == nullptr)
    throw std::runtime_error("Could not load system cursor");
}

void Cursor::setType(Type type) {
  if (m_cursor && m_freeCursor) {
    SDL_CHECK(SDL_FreeCursor(m_cursor));
  }
  m_cursor = SDL_CHECK_EXPR(SDL_CreateSystemCursor(getSDLSystemCursor(type)));
  if (m_cursor == nullptr)
    throw std::runtime_error("Could not load system cursor");
}

Cursor::~Cursor() {
  if (m_cursor && m_freeCursor) {
    SDL_CHECK(SDL_FreeCursor(m_cursor));
  }
}
}