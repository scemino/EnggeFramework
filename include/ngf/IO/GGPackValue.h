#pragma once
#include <cassert>
#include <map>
#include <memory>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <type_traits>
#include <ngf/IO/detail/GGPackValueType.hpp>
#include <ngf/IO/detail/GGPackValueIterator.hpp>
#include <ngf/IO/detail/TypeTraits.hpp>
#include <ngf/IO/detail/GGPackRef.hpp>
#include <ngf/IO/AdlSerializer.h>

namespace ngf {

// This code has been copied from https://github.com/nlohmann/json and adapted to GGPack structure
template<template<typename, typename...> class ArrayType = std::vector,
    class StringType = std::string,
    template<typename, typename, typename...> class HashType = std::map,
    template<typename> class AllocatorType= std::allocator>
class basic_ggpackvalue {
private:
  template<detail::GGPackValueType> friend
  struct detail::external_constructor;
  friend class ngf::detail::GGPackValueIterator<basic_ggpackvalue>;
  friend class ngf::detail::GGPackValueIterator<const basic_ggpackvalue>;

public:
  using value_type = basic_ggpackvalue;
  using pointer = value_type *;
  template<typename T, typename SFINAE>
  using ggpack_serializer = AdlSerializer<T, SFINAE>;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using const_reference = const value_type &;
  using iterator = ngf::detail::GGPackValueIterator<basic_ggpackvalue>;
  using const_iterator = ngf::detail::GGPackValueIterator<const basic_ggpackvalue>;
  template<typename Iterator>
  using HashIterator = ngf::detail::HashIterator<Iterator>;
  using GGPackValueType = ngf::detail::GGPackValueType;
  using initializer_list_t = std::initializer_list<detail::GGPackRef<basic_ggpackvalue>>;
  using hash_comparator_t = std::less<>;
  using hash_t =
  HashType<StringType,
           basic_ggpackvalue,
           hash_comparator_t,
           AllocatorType<std::pair<const StringType, basic_ggpackvalue>>>;
  using array_t = ArrayType<basic_ggpackvalue, AllocatorType<basic_ggpackvalue>>;
  using string_t = StringType;

private:
  detail::GGPackValueType m_type{detail::GGPackValueType::Null};

  union ggpackvalue_value {
    string_t *string_value;
    int int_value;
    double double_value;
    hash_t *hash_value;
    array_t *array_value;

    ggpackvalue_value() = default;
    /// constructor for numbers (integer)
    ggpackvalue_value(int v) noexcept: int_value(v) {}
    /// constructor for numbers (floating-point)
    ggpackvalue_value(double v) noexcept: double_value(v) {}
    /// constructor for empty values of a given type
    ggpackvalue_value(GGPackValueType t) {
      switch (t) {
      case GGPackValueType::Hash: {
        hash_value = create<hash_t>();
        break;
      }

      case GGPackValueType::Array: {
        array_value = create<array_t>();
        break;
      }

      case GGPackValueType::String: {
        string_value = create<string_t>("");
        break;
      }

      case GGPackValueType::Integer: {
        int_value = 0;
        break;
      }

      case GGPackValueType::Double: {
        double_value = 0.0;
        break;
      }

      case GGPackValueType::Null: {
        hash_value = nullptr;
        break;
      }

      default: {
        hash_value = nullptr;
        break;
      }
      }
    }

    ggpackvalue_value(const string_t &value) {
      string_value = create<string_t>(value);
    }

    ggpackvalue_value(string_t &&value) {
      string_value = create<string_t>(std::move(value));
    }

    /// constructor for objects
    ggpackvalue_value(const hash_t &value) {
      hash_value = create<hash_t>(value);
    }

    /// constructor for rvalue objects
    ggpackvalue_value(hash_t &&value) {
      hash_value = create<hash_t>(std::move(value));
    }

    /// constructor for arrays
    ggpackvalue_value(const array_t &value) {
      array_value = create<array_t>(value);
    }

    /// constructor for rvalue arrays
    ggpackvalue_value(array_t &&value) {
      array_value = create<array_t>(std::move(value));
    }

    void destroy(GGPackValueType t) noexcept {
      // flatten the current ggpack_value to a heap-allocated stack
      std::vector<basic_ggpackvalue> stack;

      // move the top-level items to stack
      if (t == GGPackValueType::Array) {
        stack.reserve(array_value->size());
        std::move(array_value->begin(), array_value->end(), std::back_inserter(stack));
      } else if (t == GGPackValueType::Hash) {
        stack.reserve(hash_value->size());
        for (auto &&it : *hash_value) {
          stack.push_back(std::move(it.second));
        }
      }

      while (!stack.empty()) {
        // move the last item to local variable to be processed
        basic_ggpackvalue current_item(std::move(stack.back()));
        stack.pop_back();

        // if current_item is array/object, move
        // its children to the stack to be processed later
        if (current_item.is_array()) {
          std::move(current_item.m_value.array->begin(), current_item.m_value.array->end(),
                    std::back_inserter(stack));

          current_item.m_value.array->clear();
        } else if (current_item.is_object()) {
          for (auto &&it : *current_item.m_value.object) {
            stack.push_back(std::move(it.second));
          }

          current_item.m_value.object->clear();
        }

        // it's now safe that current_item get destructed
        // since it doesn't have any children
      }

      switch (t) {
      case GGPackValueType::Hash: {
        AllocatorType<hash_t> alloc;
        std::allocator_traits<decltype(alloc)>::destroy(alloc, hash_value);
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, hash_value, 1);
        break;
      }

      case GGPackValueType::Array: {
        AllocatorType<array_t> alloc;
        std::allocator_traits<decltype(alloc)>::destroy(alloc, array_value);
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, array_value, 1);
        break;
      }

      case GGPackValueType::String: {
        AllocatorType<string_t> alloc;
        std::allocator_traits<decltype(alloc)>::destroy(alloc, string_value);
        std::allocator_traits<decltype(alloc)>::deallocate(alloc, string_value, 1);
        break;
      }

      default: {
        break;
      }
      }
    }
  };

  ggpackvalue_value m_value = {};

public:
  basic_ggpackvalue(const basic_ggpackvalue &value) : m_type(value.m_type) {
    switch (m_type) {
    case GGPackValueType::Null:break;
    case GGPackValueType::Hash:
      // hash
      m_value = *value.m_value.hash_value;
      break;
    case GGPackValueType::Array:
      // array
      m_value = *value.m_value.array_value;
      break;
    case GGPackValueType::String:
      // string
      m_value = *value.m_value.string_value;
      break;
    case GGPackValueType::Integer:m_value = value.m_value.int_value;
      break;
    case GGPackValueType::Double:
      // double
      m_value = value.m_value.double_value;
      break;
    }
  }

  basic_ggpackvalue(std::nullptr_t = nullptr) noexcept: m_type(GGPackValueType::Null) {}

  template<typename CompatibleType,
      typename U = detail::uncvref_t<CompatibleType>,
      std::enable_if_t<
          !detail::is_basic_ggpack<U>::value && detail::is_compatible_type<basic_ggpackvalue, U>::value, int> = 0>
  basic_ggpackvalue(CompatibleType &&val) noexcept(noexcept(
      AdlSerializer<U>::to_ggpack(std::declval<basic_ggpackvalue &>(),
                                  std::forward<CompatibleType>(val)))) {
    AdlSerializer<U>::to_ggpack(*this, std::forward<CompatibleType>(val));
  }

  basic_ggpackvalue(initializer_list_t init) {
    // check if each element is an array with two elements whose first
    // element is a string
    bool is_a_dictionary = std::all_of(init.begin(), init.end(),
                                       [](const detail::GGPackRef<basic_ggpackvalue> &element_ref) {
                                         return element_ref->isArray() && element_ref->size() == 2 &&
                                             (*element_ref)[0].isString();
                                       });
    if (is_a_dictionary) {
      // the initializer list is a list of pairs -> create object
      m_type = GGPackValueType::Hash;
      m_value = GGPackValueType::Hash;

      std::for_each(init.begin(), init.end(), [this](const detail::GGPackRef<basic_ggpackvalue> &element_ref) {
        auto element = element_ref.moved_or_copied();
        m_value.hash_value->emplace(
            std::move(*(*element.m_value.array_value)[0].m_value.string_value),
            std::move((*element.m_value.array_value)[1]));
      });
    } else {
      // the initializer list describes an array -> create array
      m_type = GGPackValueType::Array;
      m_value.array_value = create<array_t>(init.begin(), init.end());
    }
  }

  basic_ggpackvalue(basic_ggpackvalue &&other) noexcept
      : m_type(std::move(other.m_type)),
        m_value(std::move(other.m_value)) {
    // invalidate payload
    other.m_type = GGPackValueType::Null;
    other.m_value = {};
  }

  template<typename ggpackRef,
      std::enable_if_t<detail::conjunction<detail::is_ggpack_ref<ggpackRef>,
                                           std::is_same<typename ggpackRef::value_type, basic_ggpackvalue>>::value,
                       int> = 0>
  basic_ggpackvalue(const ggpackRef &ref) : basic_ggpackvalue(ref.moved_or_copied()) {}

  [[nodiscard]] inline bool isNull() const { return m_type == GGPackValueType::Null; }

  [[nodiscard]] inline bool isHash() const { return m_type == GGPackValueType::Hash; }

  [[nodiscard]] inline bool isArray() const { return m_type == GGPackValueType::Array; }

  [[nodiscard]] inline bool isString() const { return m_type == GGPackValueType::String; }

  [[nodiscard]] inline bool isInteger() const { return m_type == GGPackValueType::Integer; }

  [[nodiscard]] inline bool isDouble() const { return m_type == GGPackValueType::Double; }

  basic_ggpackvalue &operator[](size_t index) {
    if (isArray())
      return (*m_value.array_value)[index];
    throw std::logic_error("This is not an array");
  }

  const basic_ggpackvalue &operator[](size_t index) const {
    if (isArray())
      return (*m_value.array_value)[index];
    throw std::logic_error("This is not an array");
  }

  basic_ggpackvalue operator[](const std::string &key) const {
    if (m_type == GGPackValueType::Hash) {
      if (m_value.hash_value->find(key) == m_value.hash_value->end())
        return nullptr;
      return m_value.hash_value->at(key);
    }
    throw std::logic_error("This is not an hashtable");
  }

  reference operator[](const std::string &key) {
    // implicitly convert null value to an empty object
    if (isNull()) {
      m_type = GGPackValueType::Hash;
      m_value.hash_value = create<hash_t>();
    }

    // operator[] only works for hashtables
    if (isHash()) {
      return (*m_value.hash_value)[key];
    }

    throw std::logic_error("cannot use operator[] with a string argument");
  }

  basic_ggpackvalue &operator=(basic_ggpackvalue other) noexcept(
  std::is_nothrow_move_constructible<GGPackValueType>::value &&
      std::is_nothrow_move_assignable<GGPackValueType>::value &&
      std::is_nothrow_move_constructible<GGPackValueType>::value &&
      std::is_nothrow_move_assignable<GGPackValueType>::value) {
    using std::swap;
    swap(m_type, other.m_type);
    swap(m_value, other.m_value);

    // by convention, always return *this
    return *this;
  }

  iterator begin() noexcept {
    iterator result(this);
    result.setBegin();
    return result;
  }

  [[nodiscard]] const_iterator begin() const noexcept {
    return cbegin();
  }

  [[nodiscard]] const_iterator cbegin() const noexcept {
    const_iterator result(this);
    result.setBegin();
    return result;
  }

  iterator end() noexcept {
    iterator result(this);
    result.setEnd();
    return result;
  }

  [[nodiscard]] const_iterator end() const noexcept {
    return cend();
  }

  [[nodiscard]] const_iterator cend() const noexcept {
    const_iterator result(this);
    result.setEnd();
    return result;
  }

  HashIterator<iterator> items() noexcept {
    return HashIterator<iterator>(*this);
  }

  [[nodiscard]] HashIterator<const_iterator> items() const noexcept {
    return HashIterator<const_iterator>(*this);
  }

  [[nodiscard]] constexpr detail::GGPackValueType type() const noexcept {
    return m_type;
  }

  [[nodiscard]] size_t size() const noexcept {
    switch (m_type) {
    case detail::GGPackValueType::Null: {
      return 0;
    }

    case detail::GGPackValueType::Array: {
      return m_value.array_value->size();
    }

    case detail::GGPackValueType::Hash: {
      return m_value.hash_value->size();
    }

    default: {
      return 1;
    }
    }
  }

  reference at(size_t idx) {
    // at only works for arrays
    if (isArray()) {
      try {
        return m_value.array_value->at(idx);
      }
      catch (std::out_of_range &) {
        // create better exception explanation
        throw std::out_of_range("array index " + std::to_string(idx) + " is out of range");
      }
    } else {
      throw std::invalid_argument("cannot use at()");
    }
  }

  [[nodiscard]] int getInt() const {
    if (isInteger())
      return m_value.int_value;
    if (isDouble())
      return static_cast<int>(m_value.double_value);
    return 0;
  }

  [[nodiscard]] double getDouble() const {
    if (isDouble())
      return m_value.double_value;
    if (isInteger())
      return static_cast<double>(m_value.int_value);
    return 0;
  }

  [[nodiscard]] std::string getString() const {
    if (isString())
      return *m_value.string_value;
    return "";
  }

  [[nodiscard]] std::string toString() const {
    std::ostringstream s;
    s << *this;
    return s.str();
  }

  void push_back(const basic_ggpackvalue &val) {
    // push_back only works for null objects or arrays
    if (!isNull() && !isArray()) {
      throw std::logic_error("use push_back() with null or array");
    }

    // transform null object into an array
    if (isNull()) {
      m_type = GGPackValueType::Array;
      m_value = value_type::array_t{};
    }

    // add element to array
    m_value.array_value->push_back(val);
  }

  void clear() {
    // push_back only works for null objects or arrays
    if (!isNull() && !isArray()) {
      throw std::logic_error("use push_back() with null or array");
    }

    // transform null object into an array
    if (isNull()) {
      m_type = GGPackValueType::Array;
      m_value = value_type::array_t{};
    }

    // add element to array
    m_value.array_value->clear();
  }

  friend std::ostream &operator<<(std::ostream &os, const basic_ggpackvalue &value) {
    return _dumpValue(os, value, 0);
  }

private:
  /// helper for exception-safe object creation
  template<typename T, typename... Args>
  static T *create(Args &&... args) {
    AllocatorType<T> alloc;
    using AllocatorTraits = std::allocator_traits<AllocatorType<T>>;

    auto deleter = [&](T *object) {
      AllocatorTraits::deallocate(alloc, object, 1);
    };
    std::unique_ptr<T, decltype(deleter)> object(AllocatorTraits::allocate(alloc, 1), deleter);
    AllocatorTraits::construct(alloc, object.get(), std::forward<Args>(args)...);
    assert(object != nullptr);
    return object.release();
  }

  static std::ostream &_dumpValue(std::ostream &os, const basic_ggpackvalue &value, int indent) {
    if (value.isHash()) {
      _dumpHash(os, value, indent);
      return os;
    }
    if (value.isArray()) {
      _dumpArray(os, value, indent);
      return os;
    }
    if (value.isDouble()) {
      os << value.getDouble();
      return os;
    }
    if (value.isInteger()) {
      os << value.getInt();
      return os;
    }
    if (value.isNull()) {
      os << "null";
      return os;
    }
    if (value.isString()) {
      os << "\"" << value.getString() << "\"";
      return os;
    }
    return os;
  }

  static std::ostream &_dumpHash(std::ostream &os, const basic_ggpackvalue &value, int indent) {
    indent++;
    std::string padding(indent * 2, ' ');
    os << "{";
    for (auto it = value.items().begin(); it != value.items().end();) {
      os << std::endl << padding << "\"" << it.key() << "\": ";
      _dumpValue(os, it.value(), indent);
      if (++it != value.items().end()) {
        os << ",";
      }
    }
    indent--;
    padding = std::string(indent * 2, ' ');
    os << std::endl << padding << "}";
    return os;
  }

  static std::ostream &_dumpArray(std::ostream &os, const basic_ggpackvalue &value, int indent) {
    indent++;
    std::string padding(indent * 2, ' ');
    os << "[";
    for (auto iterator = value.begin(); iterator != value.end();) {
      os << std::endl << padding;
      _dumpValue(os, iterator.value(), indent);
      if (++iterator != value.end()) {
        os << ",";
      }
    }
    indent--;
    padding = std::string(indent * 2, ' ');
    os << std::endl << padding << "]";
    return os;
  }
};

using GGPackValue = basic_ggpackvalue<>;
}
