#pragma once
#include <cstdint>

namespace ngf {
namespace Util {
void endianSwap(int32_t *value);
void endianSwap(int16_t *value);
void endianSwap(uint16_t *value);
unsigned int nextPow2(unsigned int x);
}// namespace Util
}
