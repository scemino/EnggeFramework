#pragma once
#include <filesystem>
#include <vector>
#include <ngf/IO/GGPackValue.h>

namespace ngf::Json {
  GGPackValue load(const std::filesystem::path &path);
  GGPackValue parse(std::istream &input);
  GGPackValue parse(std::string_view input);
}
