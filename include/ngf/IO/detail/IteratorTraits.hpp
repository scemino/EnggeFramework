#pragma once
#include <iterator>
#include <ngf/IO/detail/VoidT.hpp>
#include <ngf/IO/detail/CppFuture.hpp>

namespace ngf::detail {
template<typename It, typename = void>
struct iterator_types {};

template<typename It>
struct iterator_types<
    It,
    void_t < typename It::difference_type, typename It::value_type, typename It::pointer,
    typename It::reference, typename It::iterator_category >> {
using difference_type = typename It::difference_type;
using value_type = typename It::value_type;
using pointer = typename It::pointer;
using reference = typename It::reference;
using iterator_category = typename It::iterator_category;
};

template<typename T, typename = void>
struct iterator_traits {
};

template<typename T>
struct iterator_traits<T, std::enable_if_t<!std::is_pointer<T>::value >>
    : iterator_types<T> {
};

template<typename T>
struct iterator_traits<T *, std::enable_if_t<std::is_object<T>::value>> {
  using iterator_category = std::random_access_iterator_tag;
  using value_type = T;
  using difference_type = ptrdiff_t;
  using pointer = T *;
  using reference = T &;
};
} // namespace ngf
