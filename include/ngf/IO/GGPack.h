#pragma once
#include <fstream>
#include <map>
#include <cstring>
#include <vector>
#include <filesystem>
#include <ngf/IO/GGPackValue.h>
#include "MemoryStream.h"

namespace ngf {
struct GGPackEntry {
  int offset;
  int size;
};

class InputGGPackEntryStream;
class GGPack {
private:
  friend InputGGPackEntryStream;

private:
  struct CaseInsensitiveCompare {
    bool operator()(const std::string &a, const std::string &b) const noexcept {
#ifdef WIN32
      return _stricmp(a.c_str(), b.c_str()) < 0;
#else
      return ::strcasecmp(a.c_str(), b.c_str()) < 0;
#endif
    }
  };

public:
  using entries = std::map<std::string, GGPackEntry, CaseInsensitiveCompare>;
  using iterator = entries::iterator;
  using const_iterator = entries::const_iterator;

public:
  GGPack() = default;
  explicit GGPack(const std::filesystem::path &path);

  void open(const std::filesystem::path &path);
  bool contains(const std::string &name);

  iterator begin() { return m_entries.begin(); }
  iterator end() { return m_entries.end(); }

  const_iterator cbegin() { return m_entries.cbegin(); }
  const_iterator cend() { return m_entries.cend(); }

private:
  std::vector<char> readEntry(const std::string &name);

private:
  void readPack();
  char *decodeUnbreakableXor(char *buffer, int length) const;

private:
  std::ifstream m_input;
  std::map<std::string, GGPackEntry, CaseInsensitiveCompare> m_entries;
  int m_method{0};
};
} // namespace ngf