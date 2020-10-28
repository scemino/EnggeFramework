#include <sstream>
#include <ngf/IO/GGPackHashReader.h>
#include <ngf/IO/GGPackValue.h>

namespace ngf {

GGPackValue GGPackHashReader::read(std::istream &input) {
  GGPackHashReader reader(input);
  std::int32_t sig = 0;
  input.read((char *) &sig, 4);

  if (sig != 0x04030201)
    throw std::logic_error("GGPack directory signature incorrect");

  reader.m_offsets = reader.readOffsets();

  // read hash
  input.seekg(12, std::ios::beg);
  return reader.readHash();
}

GGPackHashReader::GGPackHashReader(std::istream &input)
    : m_input(input) {
}

std::vector<uint32_t> GGPackHashReader::readOffsets() {
  m_input.seekg(8, std::ios::beg);
  // read ptr list offset & point to first file name offset
  size_t plo = 0;
  m_input.read((char *) &plo, 4);

  m_input.seekg(0, std::ios::end);
  auto size = static_cast<size_t>(m_input.tellg());

  if (plo < 12 || plo >= size - 4)
    throw std::logic_error("GGPack plo out of range");

  char c = 0;
  m_input.seekg(plo, std::ios::beg);
  m_input.read(&c, 1);
  if (c != 7) {
    throw std::logic_error("GGPack cannot find plo");
  }

  std::vector<uint32_t> offsets;
  do {
    uint32_t offset;
    m_input.read((char *) &offset, 4);
    if (offset == 0xFFFFFFFF)
      break;
    offsets.push_back(offset);
  } while (true);
  return offsets;
}

std::string GGPackHashReader::readString(uint32_t offset) {
  std::string key;
  auto pos = m_input.tellg();
  m_input.seekg(m_offsets[offset], std::ios::beg);
  char c;
  do {
    m_input.read(&c, 1);
    if (c == 0)
      break;
    key.append(&c, 1);
  } while (true);
  m_input.seekg(pos);
  return key;
}

GGPackValue GGPackHashReader::readValue() {
  char type;
  m_input.read((char *) &type, 1);
  switch (type) {
  case 1:
    // null
    return nullptr;
  case 2:
    // hash
    m_input.seekg(-1, std::ios::cur);
    return readHash();
  case 3:
    // array
  {
    int length = 0;
    m_input.read((char *) &length, 4);
    GGPackValue::array_t array;
    array.reserve(length);
    for (int i = 0; i < length; i++) {
      auto item = readValue();
      array.push_back(item);
    }
    char c = 0;
    m_input.read(&c, 1);
    if (c != 3)
      throw std::logic_error("unterminated array");
    return array;
  }
  case 4:
    // string
  {
    int plo_idx_int = 0;
    m_input.read((char *) &plo_idx_int, 4);
    auto string_value = readString(plo_idx_int);
    return string_value;
  }
  case 5:
  case 6: {
    // int
    // double
    int plo_idx_int = 0;
    m_input.read((char *) &plo_idx_int, 4);
    auto num_str = readString(plo_idx_int);
    if (type == 5) {
      return std::strtol(num_str.data(), nullptr, 10);
    }
    return std::strtod(num_str.data(), nullptr);
  }
  default: {
    std::stringstream s;
    s << "Not Implemented: value type " << type;
    throw std::logic_error(s.str());
  }
  }
}

GGPackValue GGPackHashReader::readHash() {
  char c = 0;
  m_input.read(&c, 1);
  if (c != 2) {
    throw std::logic_error("trying to parse a non-hash");
  }
  int n_pairs = 0;
  m_input.read((char *) &n_pairs, 4);

  GGPackValue::hash_t hash;
  for (auto i = 0; i < n_pairs; i++) {
    uint32_t key_plo_idx = 0;
    m_input.read((char *) &key_plo_idx, 4);

    auto hash_key = readString(key_plo_idx);
    auto hash_value = readValue();
    hash[hash_key] = hash_value;
  }

  m_input.read(&c, 1);
  if (c != 2)
    throw std::logic_error("unterminated hash");

  return hash;
}
}