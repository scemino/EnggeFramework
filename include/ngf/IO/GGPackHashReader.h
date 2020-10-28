#pragma once
#include <vector>
#include <ngf/IO/GGPackValue.h>
#include <ngf/IO/GGPackBufferStream.h>

namespace ngf {
class GGPackHashReader {
public:
  static GGPackValue read(std::istream &input);

private:
  GGPackHashReader(std::istream &input);

  GGPackValue readHash();
  GGPackValue readValue();
  std::vector<uint32_t> readOffsets();
  std::string readString(uint32_t offset);

private:
  std::istream &m_input;
  std::vector<uint32_t> m_offsets;
};
}

