#pragma once

namespace ngf {
#ifdef DEBUG
#define GL_CHECK(expr) do { (expr); checkGlError(__FILE__, __LINE__, #expr); } while (false)
#else
#define GL_CHECK(expr) (expr)
#endif

void checkGlError(const char *file, unsigned int line, const char *expr);
}
