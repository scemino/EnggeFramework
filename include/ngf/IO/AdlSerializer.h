#pragma once
#include <utility>
#include <ngf/IO/detail/ToGGPackValue.hpp>

namespace ngf {
template<typename T = void, typename SFINAE = void>
struct AdlSerializer;

template<typename, typename>
struct AdlSerializer {
  template<typename BasicggpackType, typename ValueType>
  static auto to_ggpack(BasicggpackType &j, ValueType &&val) noexcept(
  noexcept(::ngf::to_ggpack(j, std::forward<ValueType>(val))))
  -> decltype(::ngf::to_ggpack(j, std::forward<ValueType>(val)), void()) {
    ::ngf::to_ggpack(j, std::forward<ValueType>(val));
  }
};

}  // namespace ngf
