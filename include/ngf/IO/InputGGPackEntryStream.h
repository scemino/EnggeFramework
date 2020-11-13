#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <ngf/IO/GGPack.h>
#include <ngf/IO/MemoryStream.h>

namespace ngf {
/// @brief This is a stream of a GGPack entry.
/// @sa GGPack.
class InputGGPackEntryStream : public std::istream {
public:
  /// @brief Creates an entry stream of a specified GGPack.
  /// \param pack Pack from where to gets the stream.
  /// \param name Name of the entry to read in the GGPack.
  InputGGPackEntryStream(GGPack &pack, const std::string &name)
      : std::istream(&m_buffer), m_pack(pack), m_data(m_pack.readEntry(name)),
        m_buffer(m_data.data(), m_data.data() + m_data.size()),
        m_name(name) {
  }

private:
  GGPack &m_pack;
  std::vector<char> m_data;
  MemoryStream::membuf m_buffer;
  std::string m_name;
};
}
