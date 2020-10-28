#pragma once
#include <iostream>
#include <string>

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
}