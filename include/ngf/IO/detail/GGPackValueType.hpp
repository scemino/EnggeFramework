#pragma once

namespace ngf::detail {
enum class GGPackValueType : char {
  Null = 1,
  Hash = 2,
  Array = 3,
  String = 4,
  Integer = 5,
  Double = 6
};
}