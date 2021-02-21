#pragma once

namespace ngf::priv {
class SdlSystem {
public:
  static void ensureInit();
};

void checkSdlError(const char *file, unsigned int line, const char *expr);

template<typename T>
T checkedSdlCall(T value, const char* file, unsigned int line, const char* expr) {
  checkSdlError(file, line, expr);
  return value;
}
}

namespace ngf {
#ifdef DEBUG
  #define SDL_CHECK_EXPR(expr) ngf::priv::checkedSdlCall(expr, __FILE__, __LINE__, #expr)
  #define SDL_CHECK(expr) do { (expr); ngf::priv::checkSdlError(__FILE__, __LINE__, #expr); } while (false)
#else
  #define SDL_CHECK_EXPR(expr) (expr)
  #define SDL_CHECK(expr) (expr)
#endif
}
