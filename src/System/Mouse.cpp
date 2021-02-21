#include <ngf/System/Mouse.h>
#include <ngf/System/Window.h>
#include <SDL2/SDL.h>
#include "SdlSystem.h"

namespace ngf {
glm::ivec2 Mouse::getPosition() {
  glm::ivec2 pos;
  SDL_CHECK(SDL_GetMouseState(&pos.x, &pos.y));
  return pos * (int) ngf::Window::getDpiScale();
}

bool Mouse::isButtonPressed(Button button) {
  int id = 0;
  switch (button) {
  case Button::Left:id = SDL_BUTTON(SDL_BUTTON_LEFT);
    break;
  case Button::Middle:id = SDL_BUTTON(SDL_BUTTON_MIDDLE);
    break;
  case Button::Right:id = SDL_BUTTON(SDL_BUTTON_RIGHT);
    break;
  default:assert(false);
  }
  return SDL_CHECK_EXPR(SDL_GetMouseState(nullptr, nullptr)) & id;
}
}