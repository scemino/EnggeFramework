#pragma once
#include <type_traits>

namespace ngf::detail {
// taken from ranges-v3
template<typename T>
struct static_const
{
  static constexpr T value{};
};
}
