#include <sstream>
#include <ngf/IO/GGPackHashReader.h>
#include <ngf/IO/GGPack.h>
#include <ngf/IO/GGPackValue.h>

namespace ngf {
namespace {
constexpr unsigned char _magicBytes[] = {
    0x4f, 0xd0, 0xa0, 0xac, 0x4a, 0x5b, 0xb9, 0xe5, 0x93, 0x79, 0x45, 0xa5, 0xc1, 0xcb, 0x31, 0x93};
}

GGPack::GGPack(const std::filesystem::path &path) {
  open(path);
}
void GGPack::open(const std::filesystem::path &path) {
  m_input.open(path, std::ios::binary);
  readPack();
}

void GGPack::readPack() {
  if (!m_input.is_open())
    return;

  int dataOffset, dataSize;
  m_input.read((char *) &dataOffset, 4);
  m_input.read((char *) &dataSize, 4);

  std::vector<char> buf(dataSize);

  // try to detect correct method to decode data
  int sig = 0;
  for (m_method = 3; m_method >= 0; m_method--) {
    m_input.seekg(dataOffset, std::ios::beg);
    m_input.read(&buf[0], dataSize);
    decodeUnbreakableXor(&buf[0], dataSize);
    sig = *(int *) buf.data();
    if (sig == 0x04030201)
      break;
  }

  if (sig != 0x04030201)
    throw std::logic_error("This version of package is not supported (yet?)");

  // read hash
  m_entries.clear();

  MemoryStream is(buf.data(), buf.data() + buf.size());
  auto entries = GGPackHashReader::read(is);

  for (const auto &file : entries["files"]) {
    auto filename = file["filename"].getString();
    GGPackEntry entry{};
    entry.offset = file["offset"].getInt();
    entry.size = file["size"].getInt();
    m_entries.insert({filename, entry});
  }
}

bool GGPack::contains(const std::string &name) {
  return m_entries.find(name) != m_entries.end();
}

std::vector<char> GGPack::readEntry(const std::string &name) {
  auto entry = m_entries[name];
  std::vector<char> data;
  data.resize(entry.size + 1);
  m_input.seekg(entry.offset, std::ios::beg);

  m_input.read(data.data(), entry.size);
  decodeUnbreakableXor(data.data(), entry.size);
  data[entry.size] = 0;
  return data;
}

char *GGPack::decodeUnbreakableXor(char *buffer, int length) const {
  int code = m_method != 2 ? 0x6d : 0xad;
  char previous = length & 0xff;
  for (auto i = 0; i < length; i++) {
    auto x = (char) (buffer[i] ^ _magicBytes[i & 0xf] ^ (i * code));
    buffer[i] = (char) (x ^ previous);
    previous = x;
  }
  if (m_method != 0) {
    //Loop through in blocks of 16 and xor the 6th and 7th bytes
    int i = 5;
    while (i < length) {
      buffer[i] = (char) (buffer[i] ^ 0x0d);
      if (i + 1 < length) {
        buffer[i + 1] = (char) (buffer[i + 1] ^ 0x0d);
      }
      i += 16;
    }
  }
  return buffer;
}

} // namespace ngf
