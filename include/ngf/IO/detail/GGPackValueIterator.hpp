#pragma once
#include <cassert>
#include <iterator>
#include <ngf/IO/detail/GGPackValueType.hpp>
#include <ngf/IO/detail/CppFuture.hpp>
#include <ngf/IO/GGPackValue.h>

namespace ngf::detail {

template<typename string_type>
void int_to_string(string_type &target, std::size_t value) {
  // For ADL
  using std::to_string;
  target = to_string(value);
}

// forward declare, to be able to friend it later on
template<typename IteratorType> class HashIterator;
template<typename IteratorType> class HashIteratorValue;

template<typename GGValue>
class GGPackValueIterator {
private:
  friend GGValue;
  friend HashIterator<GGPackValueIterator>;
  friend HashIteratorValue<GGPackValueIterator>;

public:
  using value_type = typename GGValue::value_type;
  using pointer = typename std::conditional<std::is_const<GGValue>::value,
                                            typename GGValue::const_pointer,
                                            typename GGValue::pointer>::type;
  using reference = typename std::conditional<std::is_const<GGValue>::value,
                                              typename GGValue::const_reference,
                                              typename GGValue::reference>::type;
  using array_iterator_t = typename std::conditional<std::is_const<GGValue>::value,
                                                     typename GGValue::array_t::const_iterator,
                                                     typename GGValue::array_t::iterator>::type;
  using hash_iterator_t = typename std::conditional<std::is_const<GGValue>::value,
                                                    typename GGValue::hash_t::const_iterator,
                                                    typename GGValue::hash_t::iterator>::type;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::forward_iterator_tag;

private:
  pointer m_value{nullptr};
  array_iterator_t m_arrayIt{};
  hash_iterator_t m_hashIt{};

public:
  GGPackValueIterator() = default;
  explicit GGPackValueIterator(pointer value) noexcept: m_value(value) {}
  GGPackValueIterator(const GGPackValueIterator<const GGValue> &other) noexcept
      : m_value(other.m_value), m_arrayIt(other.m_arrayIt), m_hashIt(other.m_hashIt) {}
  GGPackValueIterator(const GGPackValueIterator<typename std::remove_const<GGValue>::type> &other) noexcept
      : m_value(other.m_value), m_arrayIt(other.m_arrayIt), m_hashIt(other.m_hashIt) {}

  GGPackValueIterator &operator=(const GGPackValueIterator<const GGValue> &other) noexcept {
    m_value = other.m_value;
    m_arrayIt = other.m_arrayIt;
    m_hashIt = other.m_hashIt;
    return *this;
  }

  GGPackValueIterator &operator=(const GGPackValueIterator<typename std::remove_const<GGValue>::type> &other) noexcept {
    m_value = other.m_value;
    m_arrayIt = other.m_arrayIt;
    m_hashIt = other.m_hashIt;
    return *this;
  }

  const GGPackValueIterator operator++(int) {
    auto result = *this;
    ++(*this);
    return result;
  }

  GGPackValueIterator &operator++() {
    assert(m_value != nullptr);

    switch (m_value->type()) {
    case GGPackValueType::Array: {
      std::advance(m_arrayIt, 1);
      break;
    }
    case GGPackValueType::Hash: {
      std::advance(m_hashIt, 1);
      break;
    }
    default: {
      assert(false);
    }
    }

    return *this;
  }

  const typename GGValue::hash_t::key_type &key() const {
    assert(m_value != nullptr);

    if (m_value->isHash()) {
      return m_hashIt->first;
    }

    throw std::invalid_argument("cannot use key() for non-object iterators");
  }

  reference value() const {
    return operator*();
  }

  reference operator*() const {
    assert(m_value != nullptr);

    switch (m_value->m_type) {
    case GGPackValueType::Hash: {
      assert(m_hashIt != m_value->m_value.hash_value->end());
      return m_hashIt->second;
    }

    case GGPackValueType::Array: {
      assert(m_arrayIt != m_value->m_value.array_value->end());
      return *m_arrayIt;
    }

    case GGPackValueType::Null:throw std::invalid_argument("cannot get value");

    default:throw std::invalid_argument("cannot get value");
    }
  }

private:
  void setBegin() noexcept {
    assert(m_value != nullptr);

    switch (m_value->type()) {
    case GGPackValueType::Array: {
      m_arrayIt = m_value->m_value.array_value->begin();
      break;
    }
    case GGPackValueType::Hash: {
      m_hashIt = m_value->m_value.hash_value->begin();
      break;
    }
    default: break;
    }
  }

  void setEnd() noexcept {
    assert(m_value != nullptr);

    switch (m_value->type()) {
    case GGPackValueType::Array: {
      m_arrayIt = m_value->m_value.array_value->end();
      break;
    }
    case GGPackValueType::Hash: {
      m_hashIt = m_value->m_value.hash_value->end();
      break;
    }
    default: break;
    }
  }

public:
  bool operator==(const GGPackValueIterator &rhs) const {
    switch (m_value->type()) {
    case GGPackValueType::Array: {
      return m_arrayIt == rhs.m_arrayIt;
    }
    case GGPackValueType::Hash: {
      return m_hashIt == rhs.m_hashIt;
    }
    case GGPackValueType::Null: {
      return rhs.m_value->type() == GGPackValueType::Null;
    }
    default: {
      assert(false);
    }
    }
    return false;
  }

  bool operator!=(const GGPackValueIterator &rhs) const {
    return !(*this == rhs);
  }

  reference operator*() {
    assert(m_value != nullptr);

    switch (m_value->type()) {
    case GGPackValueType::Array: {
      assert(m_arrayIt != m_value->m_value.array_value->end());
      return *m_arrayIt;
    }
    case GGPackValueType::Hash: {
      assert(m_hashIt != m_value->m_value.hash_value->end());
      return m_hashIt->second;
    }
    default: {
      assert(false);
    }
    }
    return *m_arrayIt;
  }

  pointer operator->() {
    assert(m_value != nullptr);

    switch (m_value->type()) {
    case GGPackValueType::Array: {
      assert(m_arrayIt != m_value->m_value.array_value->end());
      return &*m_arrayIt;
    }
    case GGPackValueType::Hash: {
      assert(m_hashIt != m_value->m_value.hash_value->end());
      return &*m_hashIt->second;
    }
    default: {
      assert(false);
    }
    }
  }
};

template<typename IteratorType>
class HashIteratorValue {
public:
  using difference_type = std::ptrdiff_t;
  using value_type = HashIteratorValue;
  using pointer = HashIteratorValue *;
  using reference = HashIteratorValue &;
  using iterator_category = std::input_iterator_tag;
  using string_type = typename std::remove_cv<typename std::remove_reference<decltype(std::declval<IteratorType>().key())>::type>::type;

private:
  /// the iterator
  IteratorType m_it;
  /// an index for arrays (used to create key names)
  std::size_t array_index = 0;
  /// last stringified array index
  mutable std::size_t array_index_last = 0;
  /// a string representation of the array index
  mutable string_type array_index_str = "0";
  /// an empty string (to return a reference for primitive values)
  const string_type empty_str = "";

public:
  explicit HashIteratorValue(IteratorType it) noexcept: m_it(it) {}

  HashIteratorValue &operator*() {
    return *this;
  }

  HashIteratorValue &operator++() {
    ++m_it;
    ++array_index;

    return *this;
  }

  bool operator==(const HashIteratorValue &o) const {
    return m_it.m_hashIt == o.m_it.m_hashIt;
  }

  bool operator!=(const HashIteratorValue &o) const {
    return m_it.m_hashIt != o.m_it.m_hashIt;
  }

  const string_type &key() const {
    assert(m_it.m_value != nullptr);

    switch (m_it.m_value->type()) {
      // use integer array index as key
    case GGPackValueType::Array: {
      if (array_index != array_index_last) {
        int_to_string(array_index_str, array_index);
        array_index_last = array_index;
      }
      return array_index_str;
    }

      // use key from the object
    case GGPackValueType::Hash:return m_it.key();

      // use an empty key for all primitive types
    default:return empty_str;
    }
  }

  typename IteratorType::reference value() const {
    return m_it.value();
  }
};

template<typename IteratorType>
class HashIterator {
private:
  typename IteratorType::reference container;

public:
  explicit HashIterator(typename IteratorType::reference cont) noexcept
      : container(cont) {}

  HashIteratorValue<IteratorType> begin() noexcept {
    return HashIteratorValue<IteratorType>(container.begin());
  }

  HashIteratorValue<IteratorType> end() noexcept {
    return HashIteratorValue<IteratorType>(container.end());
  }
};

template<typename T>
using uncvref_t = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

template<std::size_t N, typename IteratorType, std::enable_if_t<N == 0, int> = 0>
auto get(const detail::HashIteratorValue<IteratorType> &i) -> decltype(i.key()) {
  return i.key();
}
// Structured Bindings Support
// For further reference see https://blog.tartanllama.xyz/structured-bindings/
template<std::size_t N, typename IteratorType, std::enable_if_t<N == 1, int> = 0>
auto get(const detail::HashIteratorValue<IteratorType> &i) -> decltype(i.value()) {
  return i.value();
}
}

// The Addition to the STD Namespace is required to add
// Structured Bindings Support to the iteration_proxy_value class
// For further reference see https://blog.tartanllama.xyz/structured-bindings/
namespace std {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif
template<typename IteratorType>
class tuple_size<::ngf::detail::HashIteratorValue<IteratorType>>
    : public std::integral_constant<std::size_t, 2> {
};

template<std::size_t N, typename IteratorType>
class tuple_element<N, ::ngf::detail::HashIteratorValue<IteratorType >> {
public:
  using type = decltype(
  get<N>(std::declval<
      ::ngf::detail::HashIteratorValue<IteratorType >>()));
};

template<typename GGValue>
struct iterator_traits<ngf::detail::GGPackValueIterator<GGValue>> {
  typedef typename ngf::detail::GGPackValueIterator<GGValue>::difference_type   difference_type;
  typedef typename ngf::detail::GGPackValueIterator<GGValue>::value_type        value_type;
  typedef typename ngf::detail::GGPackValueIterator<GGValue>::pointer           pointer;
  typedef typename ngf::detail::GGPackValueIterator<GGValue>::reference         reference;
  typedef typename ngf::detail::GGPackValueIterator<GGValue>::iterator_category iterator_category;
};
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
} // namespace std