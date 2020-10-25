#include <ngf/System/StringHelper.h>
#include <codecvt>
#include <locale>

namespace ngf {
std::wstring StringHelper::towstring(const std::string &text) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  return converter.from_bytes(text.data(), text.data() + text.size());
}

std::string StringHelper::tostring(const std::wstring &text) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
  return converter.to_bytes(text);
}
}
