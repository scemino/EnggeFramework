#pragma once
#include <filesystem>
#include <ngf/IO/GGPackValue.h>
#include <ngf/IO/Json/JsonTokenReader.h>

namespace ngf::Json {
  GGPackValue load(const std::filesystem::path &path);
  GGPackValue parse(const std::vector<char> &buffer);
}
