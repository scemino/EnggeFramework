#pragma once
#include <type_traits>
#include <tuple>
#include <valarray>
#include <ngf/IO/detail/TypeTraits.hpp>
#include <ngf/IO/detail/GGPackValueType.hpp>

namespace ngf {
namespace detail {
template<GGPackValueType> struct external_constructor;

template<>
struct external_constructor<GGPackValueType::String>
{
    template<typename GGPackValue>
    static void construct(GGPackValue& j, const typename GGPackValue::string_t& s)
    {
        j.m_type = GGPackValueType::String;
        j.m_value = s;
    }

    template<typename GGPackValue>
    static void construct(GGPackValue& j, typename GGPackValue::string_t&& s)
    {
        j.m_type = GGPackValueType::String;
        j.m_value = std::move(s);
    }

    template < typename GGPackValue, typename CompatibleStringType,
            std::enable_if_t < !std::is_same<CompatibleStringType, typename GGPackValue::string_t>::value,
                    int > = 0 >
    static void construct(GGPackValue& j, const CompatibleStringType& str)
    {
        j.m_type = GGPackValueType::String;
        j.m_value.string_value = j.template create<typename GGPackValue::string_t>(str);
    }
};

template<>
struct external_constructor<GGPackValueType::Double> {
  template<typename GGPackValue>
  static void construct(GGPackValue &j, double val) noexcept {
    j.m_type = GGPackValueType::Double;
    j.m_value = val;
  }
};

template<>
struct external_constructor<GGPackValueType::Integer> {
  template<typename GGPackValue>
  static void construct(GGPackValue &j, int val) noexcept {
    j.m_type = GGPackValueType::Integer;
    j.m_value = val;
  }
};

template<>
struct external_constructor<GGPackValueType::Array>
{
    template<typename GGPackValue>
    static void construct(GGPackValue& j, const typename GGPackValue::array_t& arr)
    {
        j.m_type = GGPackValueType::Array;
        j.m_value = arr;
    }

    template<typename GGPackValue>
    static void construct(GGPackValue& j, typename GGPackValue::array_t&& arr)
    {
        j.m_type = GGPackValueType::Array;
        j.m_value = std::move(arr);
    }

    template < typename GGPackValue, typename CompatibleArrayType,
            std::enable_if_t < !std::is_same<CompatibleArrayType, typename GGPackValue::array_t>::value,
                    int > = 0 >
    static void construct(GGPackValue& j, const CompatibleArrayType& arr)
    {
        using std::begin;
        using std::end;
        j.m_type = GGPackValueType::Array;
        j.m_value.array_value = j.template create<typename GGPackValue::array_t>(begin(arr), end(arr));
    }

    template<typename GGPackValue>
    static void construct(GGPackValue& j, const std::vector<bool>& arr)
    {
        j.m_type = GGPackValueType::Array;
        j.m_value = GGPackValueType::Array;
        j.m_value.array_value->reserve(arr.size());
        for (const bool x : arr)
        {
            j.m_value.array_value->push_back(x);
        }
    }

    template<typename GGPackValue, typename T,
            std::enable_if_t<std::is_convertible<T, GGPackValue>::value, int> = 0>
    static void construct(GGPackValue& j, const std::valarray<T>& arr)
    {
        j.m_type = GGPackValueType::Array;
        j.m_value = GGPackValueType::Array;
        j.m_value.array->resize(arr.size());
        if (arr.size() > 0)
        {
            std::copy(std::begin(arr), std::end(arr), j.m_value.array->begin());
        }
        j.assert_invariant();
    }
};

template<>
struct external_constructor<GGPackValueType::Hash>
{
    template<typename GGPackValue>
    static void construct(GGPackValue& j, const typename GGPackValue::hash_t& obj)
    {
        j.m_type = GGPackValueType::Hash;
        j.hash_value = obj;
    }

    template<typename GGPackValue>
    static void construct(GGPackValue& j, typename GGPackValue::hash_t&& obj)
    {
        j.m_type = GGPackValueType::Hash;
        j.m_value = std::move(obj);
    }

    template < typename GGPackValue, typename CompatibleObjectType,
            std::enable_if_t < !std::is_same<CompatibleObjectType, typename GGPackValue::hash_t>::value, int > = 0 >
    static void construct(GGPackValue& j, const CompatibleObjectType& obj)
    {
        using std::begin;
        using std::end;

        j.m_type = GGPackValueType::Hash;
        j.m_value.hash_value = j.template create<typename GGPackValue::hash_t>(begin(obj), end(obj));
    }
};

/////////////
// to_ggpack //
/////////////

template<typename GGPackValue, typename CompatibleString,
        std::enable_if_t<std::is_constructible<typename GGPackValue::string_t, CompatibleString>::value, int> = 0>
void to_ggpack(GGPackValue& j, const CompatibleString& s)
{
    external_constructor<GGPackValueType::String>::construct(j, s);
}

template<typename GGPackValue>
void to_ggpack(GGPackValue& j, typename GGPackValue::string_t&& s)
{
    external_constructor<GGPackValueType::String>::construct(j, std::move(s));
}

template<typename GGPackValue, typename FloatType,
        std::enable_if_t<std::is_floating_point<FloatType>::value, int> = 0>
void to_ggpack(GGPackValue& j, FloatType val) noexcept
{
    external_constructor<GGPackValueType::Double>::construct(j, static_cast<double>(val));
}

template<typename GGPackValue, typename CompatibleNumberIntegerType,
        std::enable_if_t<is_compatible_integer_type<int, CompatibleNumberIntegerType>::value, int> = 0>
void to_ggpack(GGPackValue& j, CompatibleNumberIntegerType val) noexcept
{
    external_constructor<GGPackValueType::Integer>::construct(j, static_cast<int>(val));
}

template < typename GGPackValue, typename CompatibleArrayType,
        std::enable_if_t < is_compatible_array_type<GGPackValue,
                CompatibleArrayType>::value&&
                      !is_compatible_object_type<GGPackValue, CompatibleArrayType>::value&&
                      !is_compatible_string_type<GGPackValue, CompatibleArrayType>::value&&
                      !std::is_same<typename GGPackValue::binary_t, CompatibleArrayType>::value&&
                      !is_basic_ggpack<CompatibleArrayType>::value,
                int > = 0 >
void to_ggpack(GGPackValue& j, const CompatibleArrayType& arr)
{
    external_constructor<GGPackValueType::Array>::construct(j, arr);
}

template<typename GGPackValue>
void to_ggpack(GGPackValue& j, typename GGPackValue::array_t&& arr)
{
    external_constructor<GGPackValueType::Array>::construct(j, std::move(arr));
}

template <typename GGPackValue, typename CompatibleObjectType,
        std::enable_if_t <is_compatible_object_type<GGPackValue, CompatibleObjectType>::value&& !is_basic_ggpack<CompatibleObjectType>::value, int > = 0 >
void to_ggpack(GGPackValue& j, const CompatibleObjectType& obj)
{
    external_constructor<GGPackValueType::Hash>::construct(j, obj);
}

template <typename GGPackValue>
void to_ggpack(GGPackValue& j, typename GGPackValue::hash_t&& obj)
{
    external_constructor<GGPackValueType::Hash>::construct(j, std::move(obj));
}

template <typename GGPackValue, typename T, std::size_t N,
        std::enable_if_t < !std::is_constructible<typename GGPackValue::string_t,
                const T(&)[N]>::value,
                int > = 0 >
void to_ggpack(GGPackValue& j, const T(&arr)[N])
{
    external_constructor<GGPackValueType::Array>::construct(j, arr);
}

template <typename GGPackValue, typename T1, typename T2, std::enable_if_t < std::is_constructible<GGPackValue, T1>::value&& std::is_constructible<GGPackValue, T2>::value, int > = 0 >
void to_ggpack(GGPackValue& j, const std::pair<T1, T2>& p)
{
    j = { p.first, p.second };
}

// for https://github.com/nlohmann/ggpack/pull/1134
template<typename BasicggpackType, typename T,
        std::enable_if_t<std::is_same<T, HashIteratorValue<typename BasicggpackType::iterator>>::value, int> = 0>
void to_ggpack(BasicggpackType& j, const T& b)
{
    j = { {b.key(), b.value()} };
}

template<typename BasicggpackType, typename Tuple, std::size_t... Idx>
void to_ggpack_tuple_impl(BasicggpackType& j, const Tuple& t, std::index_sequence<Idx...> /*unused*/)
{
    j = { std::get<Idx>(t)... };
}

template<typename BasicggpackType, typename T, std::enable_if_t<is_constructible_tuple<BasicggpackType, T>::value, int > = 0>
void to_ggpack(BasicggpackType& j, const T& t)
{
    to_ggpack_tuple_impl(j, t, std::make_index_sequence<std::tuple_size<T>::value> {});
}

struct to_ggpack_fn {
  template<typename BasicggpackType, typename T>
  auto operator()(BasicggpackType &j, T &&val) const noexcept(noexcept(to_ggpack(j, std::forward<T>(val))))
  -> decltype(to_ggpack(j, std::forward<T>(val)), void()) {
    return to_ggpack(j, std::forward<T>(val));
  }
};
}

/// namespace to hold default `to_ggpack` function
namespace {
constexpr const auto &to_ggpack = detail::static_const<detail::to_ggpack_fn>::value;
} // namespace
}
