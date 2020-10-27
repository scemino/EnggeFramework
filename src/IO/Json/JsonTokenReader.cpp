#include <IO/Json/JsonTokenReader.h>

namespace ngf::Json {
TokenReader::TokenReader() = default;

bool TokenReader::load(const std::filesystem::path &path) {
  std::ifstream is;
  is.open(path);

  if (!is.is_open())
    return false;

  is.seekg(-1, std::ios::end);
  auto size = is.tellg();
  is.seekg(0, std::ios::beg);
  std::vector<char> buffer(size);
  is.read(buffer.data(), size);

#if 0
  std::ofstream o;
  o.open(path);
  o.write(buffer.data(), buffer.size());
  o.close();
#endif

  m_stream.setBuffer(buffer);
  return true;
}

void TokenReader::parse(const std::vector<char> &content) {
  m_stream.setBuffer(content);
}

TokenId TokenReader::readString() {
  while (isalpha(m_stream.peek())) {
    m_stream.ignore();
  }
  return TokenId::String;
}

TokenId TokenReader::readQuotedString() {
  m_stream.ignore(std::numeric_limits<std::streamsize>::max(), '\"');
  return TokenId::String;
}

TokenId TokenReader::readNumber() {
  while (isdigit(m_stream.peek())) {
    m_stream.ignore();
  }
  if (m_stream.peek() == '.') {
    m_stream.ignore();
  }
  while (isdigit(m_stream.peek())) {
    m_stream.ignore();
  }
  return TokenId::Number;
}

TokenId TokenReader::readTokenId() {
  if (m_stream.eof()) {
    return TokenId::End;
  }
  char c = 0;
  m_stream.read(&c, 1);

  switch (c) {
  case '\0':return TokenId::End;
  case '\n':return TokenId::NewLine;
  case '\t':
  case ' ':
    while (isspace(m_stream.peek()) && m_stream.peek() != '\n')
      m_stream.ignore();
    return TokenId::Whitespace;
  case ':':return TokenId::Colon;
  case ',':return TokenId::Comma;
  case '{':return TokenId::StartHash;
  case '}':return TokenId::EndHash;
  case '\"':return readQuotedString();
  default:
    if (c == '-' || isdigit(c)) {
      return readNumber();
    } else if (isalpha(c)) {
      return readString();
    }
    //error("unknown character: {}", c);
    return TokenId::None;
  }
}

std::string TokenReader::readText(std::streampos pos, std::streamsize size) {
  std::string out;
  out.reserve(size);
  m_stream.seek(pos);
  char c;
  for (auto i = 0; i < static_cast<int>(size); i++) {
    m_stream.read(&c, 1);
    out.append(&c, 1);
  }
  return out;
}

std::string TokenReader::readText(const Token &token) {
  return readText(token.start, token.end - token.start);
}

bool TokenReader::readToken(Token &token) {
  std::streampos start = m_stream.tell();
  auto id = readTokenId();
  while (id == TokenId::Whitespace || id == TokenId::Comment || id == TokenId::NewLine || id == TokenId::None) {
    start = m_stream.tell();
    id = readTokenId();
  }
  std::streampos end = m_stream.tell();
  token.id = id;
  token.start = start;
  token.end = end;
  return true;
}

TokenReader::Iterator::Iterator(TokenReader &reader, std::streampos pos)
    : _reader(reader),
      _pos(pos) {
  operator++();
}

TokenReader::Iterator::Iterator(const Iterator &it)
    : _reader(it._reader), _pos(it._pos), _token(it._token) {
}

TokenReader::Iterator &TokenReader::Iterator::operator++() {
  _reader.m_stream.seek(_pos);
  _reader.readToken(_token);
  _pos = _reader.m_stream.tell();
  return *this;
}

TokenReader::Iterator TokenReader::Iterator::operator++(int) {
  Iterator tmp(*this);
  operator++();
  return tmp;
}

Token &TokenReader::Iterator::operator*() {
  return _token;
}

const Token &TokenReader::Iterator::operator*() const {
  return _token;
}

Token *TokenReader::Iterator::operator->() {
  return &_token;
}

TokenReader &TokenReader::Iterator::getReader() {
  return _reader;
}

TokenReader::iterator TokenReader::begin() {
  return Iterator(*this, 0);
}

TokenReader::iterator TokenReader::end() {
  auto start = m_stream.tell();
  m_stream.seek(m_stream.getLength());
  auto pos = m_stream.tell();
  m_stream.seek(start);
  return Iterator(*this, pos);
}

std::string Token::readToken() const {
  switch (id) {
  case TokenId::NewLine:return "NewLine";
  case TokenId::Colon:return "Colon";
  case TokenId::End:return "End";
  case TokenId::None:return "None";
  case TokenId::Number:return "Number";
  case TokenId::String:return "String";
  case TokenId::Whitespace:return "Whitespace";
  case TokenId::StartHash:return "{";
  case TokenId::EndHash:return "}";
  default:return "?";
  }
}

std::ostream &operator<<(std::ostream &os, const Token &obj) {
  os << obj.readToken();
  return os;
}

} // namespace ngf
