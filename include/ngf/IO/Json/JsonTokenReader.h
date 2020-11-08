#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <ngf/IO/GGPackBufferStream.h>

namespace ngf::Json {
enum class TokenId {
  None,
  NewLine,
  Number,
  Whitespace,
  Colon,
  Comma,
  String,
  Comment,
  StartHash,
  EndHash,
  StartArray,
  EndArray,
  End
};

struct Token {
  TokenId id;
  std::streampos start;
  std::streampos end;

  friend std::ostream &operator<<(std::ostream &os, const Token &obj);

private:
  [[nodiscard]] std::string readToken() const;
};

class TokenReader {
public:
  class Iterator {
  public:
    using value_type = Token;
    using difference_type = ptrdiff_t;
    using pointer = Token *;
    using reference = Token &;
    using iterator_category = std::forward_iterator_tag;

  private:
    TokenReader &_reader;
    std::streampos _pos;
    Token _token;

  public:
    Iterator(TokenReader &reader, std::streampos pos);
    Iterator(const Iterator &it);
    Iterator &operator++();
    Iterator operator++(int);

    TokenReader &getReader();

    bool operator==(const Iterator &rhs) const { return _pos == rhs._pos; }
    bool operator!=(const Iterator &rhs) const { return _pos != rhs._pos; }
    Token &operator*();
    const Token &operator*() const;
    Token *operator->();
  };

  using iterator = Iterator;

public:
  TokenReader();

  bool load(const std::filesystem::path &path);
  void parse(const std::vector<char> &content);
  iterator begin();
  iterator end();
  bool readToken(Token &token);
  std::string readText(std::streampos pos, std::streamsize size);
  std::string readText(const Token &token);

private:
  TokenId readTokenId();
  TokenId readNumber();
  TokenId readString();
  TokenId readQuotedString();

private:
  GGPackBufferStream m_stream;
};

} // namespace ngf::Json
