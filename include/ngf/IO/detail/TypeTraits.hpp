#pragma once
#include <type_traits>
#include <ngf/IO/GGPackValue.h>
#include <ngf/IO/detail/CppFuture.hpp>
#include <ngf/IO/detail/Detected.hpp>
#include <ngf/IO/detail/IteratorTraits.hpp>

namespace ngf {
template<template<typename, typename...> class ArrayType,
    class StringType,
    template<typename, typename, typename...> class HashType,
    template<typename> class AllocatorType>
class basic_ggpackvalue;
}

namespace ngf::detail {

template<typename T>
using mapped_type_t = typename T::mapped_type;

template<typename T>
using key_type_t = typename T::key_type;

template<typename T>
using value_type_t = typename T::value_type;

template<typename T>
using difference_type_t = typename T::difference_type;

template<typename T>
using pointer_t = typename T::pointer;

template<typename T>
using reference_t = typename T::reference;

template<typename T>
using iterator_category_t = typename T::iterator_category;

template<typename T>
using iterator_t = typename T::iterator;

template<typename> struct is_basic_ggpack : std::false_type {};

template<template<typename, typename...> class ArrayType,
    class StringType,
    template<typename, typename, typename...> class HashType,
    template<typename> class AllocatorType>
struct is_basic_ggpack<ngf::basic_ggpackvalue<ArrayType, StringType, HashType, AllocatorType>> : std::true_type {};

template<typename>
class GGPackRef;

template<typename T, typename = void>
struct is_iterator_traits : std::false_type {};

template<typename T>
struct is_iterator_traits<iterator_traits < T>> {
private:
using traits = iterator_traits<T>;

public:
static constexpr auto value =
    is_detected<value_type_t, traits>::value &&
        is_detected<difference_type_t, traits>::value &&
        is_detected<pointer_t, traits>::value &&
        is_detected<iterator_category_t, traits>::value &&
        is_detected<reference_t, traits>::value;
};

template<typename>
struct is_ggpack_ref : std::false_type {};

template<typename T>
struct is_ggpack_ref<GGPackRef <T>> : std::true_type {
};

template<typename T, typename... Args>
using to_ggpack_function = decltype(T::to_ggpack(std::declval<Args>()...));

template<typename BasicGGPackValueType, typename T, typename = void>
struct has_to_ggpack : std::false_type {};

template<typename BasicGGPackValueType, typename T>
struct has_to_ggpack<BasicGGPackValueType, T, std::enable_if_t<!is_basic_ggpack<T>::value >> {
  using serializer = typename BasicGGPackValueType::template ggpack_serializer<T, void>;

  static constexpr bool value =
      is_detected_exact<void, to_ggpack_function, serializer, BasicGGPackValueType &,
                        T>::value;
};

template<typename T, typename = void>
struct is_complete_type : std::false_type {};

template<typename T>
struct is_complete_type<T, decltype(void(sizeof(T)))> : std::true_type {};

template<typename RealIntegerType, typename CompatibleNumberIntegerType,
    typename = void>
struct is_compatible_integer_type_impl : std::false_type {};

template<typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type_impl<
    RealIntegerType, CompatibleNumberIntegerType,
    std::enable_if_t<std::is_integral<RealIntegerType>::value &&
        std::is_integral<CompatibleNumberIntegerType>::value &&
        !std::is_same<bool, CompatibleNumberIntegerType>::value >> {
// is there an assert somewhere on overflows?
  using RealLimits = std::numeric_limits<RealIntegerType>;
  using CompatibleLimits = std::numeric_limits<CompatibleNumberIntegerType>;

  static constexpr auto value =
      std::is_constructible<RealIntegerType,
                            CompatibleNumberIntegerType>::value &&
          CompatibleLimits::is_integer &&
          RealLimits::is_signed == CompatibleLimits::is_signed;
};

template<typename RealIntegerType, typename CompatibleNumberIntegerType>
struct is_compatible_integer_type
    : is_compatible_integer_type_impl<RealIntegerType,
                                      CompatibleNumberIntegerType> {
};

template<typename BasicGGPackValueType, typename CompatibleType, typename = void>
struct is_compatible_type_impl : std::false_type {};

template<typename BasicGGPackValueType, typename CompatibleType>
struct is_compatible_type_impl<
    BasicGGPackValueType, CompatibleType,
    std::enable_if_t<is_complete_type<CompatibleType>::value >> {
  static constexpr bool value =
      has_to_ggpack<BasicGGPackValueType, CompatibleType>::value;
};

// is_compatible_array_type
template<typename BasicGGPackValueType, typename CompatibleArrayType, typename = void>
struct is_compatible_array_type_impl : std::false_type {};

template<typename BasicGGPackValueType, typename CompatibleArrayType>
struct is_compatible_array_type_impl<
    BasicGGPackValueType, CompatibleArrayType,
    std::enable_if_t<is_detected<value_type_t, CompatibleArrayType>::value &&
        is_detected<iterator_t, CompatibleArrayType>::value &&
        !is_iterator_traits <
            iterator_traits < CompatibleArrayType >>::value >>
{
static constexpr bool value =
    std::is_constructible<BasicGGPackValueType,
                          typename CompatibleArrayType::value_type>::value;
};

template<typename BasicGGPackValueType, typename CompatibleArrayType>
struct is_compatible_array_type
    : is_compatible_array_type_impl<BasicGGPackValueType, CompatibleArrayType> {
};

// is_compatible_type
template<typename BasicGGPackValueType, typename CompatibleType>
struct is_compatible_type
    : is_compatible_type_impl<BasicGGPackValueType, CompatibleType> {
};

template<typename BasicGGPackValueType, typename CompatibleObjectType,
    typename = void>
struct is_compatible_object_type_impl : std::false_type {};

template<typename BasicGGPackValueType, typename CompatibleObjectType>
struct is_compatible_object_type_impl<
    BasicGGPackValueType, CompatibleObjectType,
    std::enable_if_t<is_detected<mapped_type_t, CompatibleObjectType>::value &&
        is_detected<key_type_t, CompatibleObjectType>::value >> {

  using hash_t = typename BasicGGPackValueType::hash_t;

  // macOS's is_constructible does not play well with nonesuch...
  static constexpr bool value =
      std::is_constructible<typename hash_t::key_type,
                            typename CompatibleObjectType::key_type>::value &&
          std::is_constructible<typename hash_t::mapped_type,
                                typename CompatibleObjectType::mapped_type>::value;
};

template<typename BasicGGPackValueType, typename CompatibleObjectType>
struct is_compatible_object_type
    : is_compatible_object_type_impl<BasicGGPackValueType, CompatibleObjectType> {
};

template<typename BasicGGPackValueType, typename CompatibleStringType,
    typename = void>
struct is_compatible_string_type_impl : std::false_type {};

template<typename BasicGGPackValueType, typename CompatibleStringType>
struct is_compatible_string_type_impl<
    BasicGGPackValueType, CompatibleStringType,
    std::enable_if_t<is_detected_exact<typename BasicGGPackValueType::string_t::value_type,
                                       value_type_t, CompatibleStringType>::value >> {
  static constexpr auto value =
      std::is_constructible<typename BasicGGPackValueType::string_t, CompatibleStringType>::value;
};

template<typename BasicGGPackValueType, typename ConstructibleStringType>
struct is_compatible_string_type
    : is_compatible_string_type_impl<BasicGGPackValueType, ConstructibleStringType> {
};

// https://en.cppreference.com/w/cpp/types/conjunction
template<class...> struct conjunction : std::true_type {};
template<class B1> struct conjunction<B1> : B1 {};
template<class B1, class... Bn>
struct conjunction<B1, Bn...>
    : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {
};

template<typename T1, typename T2>
struct is_constructible_tuple : std::false_type {};

template<typename T1, typename... Args>
struct is_constructible_tuple<T1, std::tuple<Args...>> : conjunction<std::is_constructible<T1, Args>...> {};

}