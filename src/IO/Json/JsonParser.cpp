#include <IO/Json/JsonParser.h>

namespace ngf::Json {

namespace {
GGPackValue readHash(TokenReader::iterator &it);
GGPackValue parse(ngf::Json::TokenReader &reader);
GGPackValue parse(TokenReader::iterator &it);

GGPackValue readHash(TokenReader::iterator &it) {
  GGPackValue::hash_t hash;
  auto &reader = it.getReader();
  while (it != reader.end() && it->id != TokenId::EndHash) {
    auto token = *it++;
    auto key = reader.readText(token);
    // remove "" if any
    if (key.length() > 0 && key[0] == '\"' && key[key.length() - 1] == '\"') {
      key = key.substr(1, key.length() - 2);
    }
    // skip colon
    it++;
    hash[key] = parse(it);
    if (it->id == TokenId::Comma) {
      it++;
    }
  }
  it++;
  return hash;
}

GGPackValue readArray(TokenReader::iterator &it) {
  GGPackValue::array_t array;
  auto &reader = it.getReader();
  while (it != reader.end() && it->id != TokenId::EndArray) {
    array.push_back(parse(it));
    // skip comma
    if (it->id == TokenId::Comma) {
      it++;
    }
  }
  it++;
  return array;
}

GGPackValue parse(ngf::Json::TokenReader &reader) {
  auto it = reader.begin();
  auto token = *it;
  if (token.id == TokenId::StartHash) {
    if (it != reader.end()) {
      return parse(it);
    }
    return nullptr;
  }
  return readHash(it);
}

GGPackValue parse(TokenReader::iterator &it) {
  auto &reader = it.getReader();
  auto token = *it;
  it++;
  switch (token.id) {
  case TokenId::StartHash: {
    return readHash(it);
  }
  case TokenId::StartArray: {
    return readArray(it);
  }
  case TokenId::Number: {
    auto text = reader.readText(token);
    return std::strtod(text.data(), nullptr);
  }
  case TokenId::String: {
    auto text = reader.readText(token);
    // remove "" if any
    if (text.length() > 0 && text[0] == '\"' && text[text.length() - 1] == '\"') {
      return text.substr(1, text.length() - 2);
    }
    break;
  }
  default:break;
  }
  return nullptr;
}
}

GGPackValue load(const std::filesystem::path &path) {
  ngf::Json::TokenReader reader;
  if (!reader.load(path))
    return nullptr;

  return parse(reader);
}

GGPackValue parse(std::istream &input) {
  ngf::Json::TokenReader reader;
  input.seekg(0,std::ios::end);
  auto size = input.tellg();
  input.seekg(0,std::ios::beg);
  std::vector<char> buf(size);
  input.read(buf.data(), size);

  reader.parse(buf);
  return parse(reader);
}
}
