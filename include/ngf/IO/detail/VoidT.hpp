#pragma once

namespace ngf {
namespace detail {
template<typename ...Ts> struct make_void {
  using type = void;
};
template<typename ...Ts> using void_t = typename make_void<Ts...>::type;
} // namespace detail
}  // namespace ngf
