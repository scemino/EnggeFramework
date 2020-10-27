#pragma once
#include <initializer_list>
#include <utility>
#include <ngf/IO/detail/TypeTraits.hpp>

namespace ngf::detail {
template<typename GGPackValue>
class GGPackRef {
public:
  using value_type = GGPackValue;

  GGPackRef(value_type &&value)
      : owned_value(std::move(value)), value_ref(&owned_value), is_rvalue(true) {}

  GGPackRef(const value_type &value)
      : value_ref(const_cast<value_type *>(&value)), is_rvalue(false) {}

  GGPackRef(std::initializer_list<GGPackRef> init)
      : owned_value(init), value_ref(&owned_value), is_rvalue(true) {}

  template<class... Args, std::enable_if_t<std::is_constructible<value_type, Args...>::value, int> = 0>
  GGPackRef(Args &&... args)
      : owned_value(std::forward<Args>(args)...), value_ref(&owned_value), is_rvalue(true) {}

  // class should be movable only
  GGPackRef(GGPackRef &&) = default;
  GGPackRef(const GGPackRef &) = delete;
  GGPackRef &operator=(const GGPackRef &) = delete;
  GGPackRef &operator=(GGPackRef &&) = delete;
  ~GGPackRef() = default;

  value_type moved_or_copied() const {
    if (is_rvalue) {
      return std::move(*value_ref);
    }
    return *value_ref;
  }

  value_type const &operator*() const {
    return *static_cast<value_type const *>(value_ref);
  }

  value_type const *operator->() const {
    return static_cast<value_type const *>(value_ref);
  }

private:
  mutable value_type owned_value = nullptr;
  value_type *value_ref = nullptr;
  const bool is_rvalue = true;
};
}  // namespace ngf
