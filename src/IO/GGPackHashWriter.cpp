#include <algorithm>
#include <sstream>
#include <ngf/IO/GGPackHashWriter.h>

namespace ngf {
void GGPackHashWriter::writeHash(const GGPackValue &hash, std::ostream &os) {
  if (hash.type() != detail::GGPackValueType::Hash)
    throw std::logic_error("trying to write a non-hash");
  char type = static_cast<char>(hash.type());
  os.write(&type, 1);

  auto n_pairs = static_cast<int32_t>(hash.size());
  os.write((char *) &n_pairs, 4);

  for (auto &value: hash.items()) {
    writeString(value.key(), os);
    writeValue(value.value(), os);
  }

  // terminate hash
  os.write(&type, 1);
}

void GGPackHashWriter::writeArray(const GGPackValue &array, std::ostream &os) {
  if (array.type() != detail::GGPackValueType::Array)
    throw std::logic_error("trying to write a non-array");
  char type = static_cast<char>(array.type());
  os.write(&type, 1);

  auto n_pairs = static_cast<int32_t>(array.size());
  os.write((char *) &n_pairs, 4);

  for (const auto &value: array) {
    writeValue(value, os);
  }

  // terminate array
  os.write(&type, 1);
}

void GGPackHashWriter::writeString(const std::string &key, std::ostream &os) {
  auto it = m_keys.find(key);
  int32_t index;
  if (it == m_keys.end()) {
    index = m_index;
    m_keys.insert({key, m_index++});
  } else {
    index = it->second;
  }
  os.write((char *) &index, 4);
}

void GGPackHashWriter::writeValue(const GGPackValue &value, std::ostream &os) {
  switch (value.type()) {
  case detail::GGPackValueType::Null: {
    char type = 1;
    os.write(&type, 1);
    break;
  }
  case detail::GGPackValueType::Hash:writeHash(value, os);
    break;
  case detail::GGPackValueType::Array:writeArray(value, os);
    break;
  case detail::GGPackValueType::String: {
    char type = static_cast<char>(value.type());
    os.write(&type, 1);
    writeString(value.getString(), os);
    break;
  }
  case detail::GGPackValueType::Integer: {
    char type = static_cast<char>(value.type());
    os.write(&type, 1);
    std::ostringstream num;
    num << value.getInt();
    writeString(num.str(), os);
    break;
  }
  case detail::GGPackValueType::Double: {
    char type = static_cast<char>(value.type());
    os.write(&type, 1);
    std::ostringstream num;
    num << value.getDouble();
    writeString(num.str(), os);
    break;
  }
  }
}

void GGPackHashWriter::write(const ngf::GGPackValue &hash, std::ostream &os) {
  GGPackHashWriter writer;
  writer.m_keys.clear();
  // off 0: write signature
  int32_t tmp = 0x04030201;
  os.write((char *) &tmp, 4);
  // off 4: ??
  tmp = 0;
  os.write((char *) &tmp, 4);
  // off 8: write future plo
  os.write((char *) &tmp, 4);
  // off 12: write hash
  writer.writeHash(hash, os);
  // write strings
  auto stringsOffset = static_cast<int32_t>(os.tellp());
  char nullChar = '\0';
  std::vector<std::string> keys;
  keys.resize(writer.m_keys.size());
  for (const auto &key : writer.m_keys) {
    keys[key.second] = key.first;
  }
  for (const auto &key : keys) {
    os.write(key.data(), key.length());
    os.write(&nullChar, 1);
  }

  // write pointer list
  auto ploOffset = static_cast<int32_t>(os.tellp());
  char c = 7;
  os.write((char *) &c, 1);
  for (const auto &key : keys) {
    os.write((char *) &stringsOffset, 4);
    stringsOffset += key.length() + 1;
  }
  stringsOffset = 0xFFFFFFFF;
  os.write((char *) &stringsOffset, 4);

  // change pointer list offset
  os.seekp(8, std::ios_base::beg);
  os.write((char *) &ploOffset, 4);
}
}
