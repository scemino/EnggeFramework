#include <ngf/IO/Json/JsonToken.h>

namespace ngf::Json {
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
}

