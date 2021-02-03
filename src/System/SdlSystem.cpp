#include "SdlSystem.h"
#include <atomic>
#include <sstream>
#include <SDL2/SDL.h>

namespace ngf::priv {
namespace {
static std::atomic_int g_loaded{0};
}

void SdlSystem::ensureInit() {
  if (g_loaded.fetch_add(1) != 0)
    return;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
    std::ostringstream ss;
    ss << "Error when initializing SDL (error=" << SDL_GetError() << ")";
    throw std::runtime_error(ss.str());
  }

  // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.3 Core + GLSL 150
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#else
  // GL 3.0 + GLSL 130
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
}
}
