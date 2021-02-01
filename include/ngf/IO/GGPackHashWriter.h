#pragma once
#include <string>
#include <vector>
#include <ngf/IO/GGPackValue.h>

namespace ngf {
class GGPackHashWriter {
public:
  static void write(const GGPackValue &hash, std::ostream &os);

private:
  void writeHash(const GGPackValue &hash, std::ostream &os);
  void writeArray(const GGPackValue &hash, std::ostream &os);
  void writeValue(const GGPackValue &value, std::ostream &os);
  void writeString(const std::string &key, std::ostream &os);

private:
  std::vector<std::string> m_keys;
};
}
