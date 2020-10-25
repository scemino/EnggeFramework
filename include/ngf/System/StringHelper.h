#pragma once
#include <string>

namespace ngf {
class StringHelper {
public:
  static std::wstring towstring(const std::string &text);
  static std::string tostring(const std::wstring &text);
};
}
